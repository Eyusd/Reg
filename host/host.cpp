#include <openenclave/host.h>
#include <stdio.h>
#include "linereg_u.h"

int main(int argc, const char* argv[])
{
    oe_result_t result;
    int ret = 1;
    oe_enclave_t* enclave = NULL;

    //Program loop
    char choice;
    char useless;
    bool run = true;
    //Train
        FILE *csv_file;
        double values_t[9];
        double expected;
        bool ok;
        bool allgood = true;
    //Infer
        double values[9];
        double output;

    uint32_t flags = OE_ENCLAVE_FLAG_DEBUG;
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s enclave_image_path [ --simulate  ]\n", argv[0]);
        goto exit;
    }
    // Create the enclave
    result = oe_create_linereg_enclave(argv[1], OE_ENCLAVE_TYPE_AUTO, flags, NULL, 0, &enclave);
    if (result != OE_OK)
    {
        fprintf(stderr, "oe_create_linereg_enclave(): result=%u (%s)\n", result, oe_result_str(result));
        goto exit;
    }


    //Run program
    result = enclave_init(enclave);
    if (result != OE_OK)
    {
        fprintf(stderr, "calling into enclave_linereg failed: result=%u (%s)\n", result, oe_result_str(result));
        goto exit;
    }
    enclave_new_to_old(enclave);

    while (run)
    {
        fprintf(stderr, "\n");    
        fprintf(stderr, "Train, Infer, or Exit ? [t/i/e] ");
        scanf("%c%c", &choice, &useless);
        switch (choice)
        {
            case 't':
                enclave_new_to_old(enclave);
                csv_file = fopen("trainingset.csv", "r");
                // Trained with coeffs = 1
                while (fscanf(csv_file, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", &values_t[0],&values_t[1],&values_t[2],&values_t[3],&values_t[4],&values_t[5],&values_t[6],&values_t[7],&values_t[8],&expected) == 10)
                {
                    enclave_train(enclave, values_t, expected, &ok);
                    if (!ok)
                    {
                        allgood = false;
                        break;
                    }
                }
                if (allgood)
                {
                    fprintf(stderr, "Training set registered successfully \n");
                    enclave_new_to_old(enclave);
                }
                else
                {
                    fprintf(stderr, "Something went wrong \n");
                    enclave_old_to_new(enclave);
                }
                break;
            
            case 'i':
                scanf("%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5], &values[6], &values[7], &values[8]);

                enclave_infer(enclave, values, &output);
                fprintf(stderr, "%lf \n", output);

                break;
            
            case 'e':
                run = false;
                break;
        }
    }

    ret = 0;

exit:
    // Clean up the enclave if we created one
    if (enclave)
        oe_terminate_enclave(enclave);

    return ret;
}
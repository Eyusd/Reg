// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#pragma once
#include <openenclave/enclave.h>
#include <string>
#include "attestation.h"
#include "crypto.h"
#include "regression.h"

using namespace std;

class ecall_dispatcher
{
  private:
    bool m_initialized;
    Crypto* m_crypto;
    Attestation* m_attestation;
    string m_name;
    ecall_regression m_regression;

  public:
    ecall_dispatcher(const char* name);
    ~ecall_dispatcher();
    int get_remote_report_with_pubkey(
        uint8_t** pem_key,
        size_t* key_size,
        uint8_t** remote_report,
        size_t* remote_report_size);
    void reg_initialize();
    double reg_infer(double values[9]);
    double reg_train(double values[9], double expected);
    void reg_new_to_old();
    void reg_old_to_new();

  private:
    bool initialize(const char* name);
};
/*
 * This file is part of nmealib.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <nmealib/util.h>
#include <CUnit/Basic.h>
#include <stdlib.h>

extern int contextSuiteSetup(void);
extern int generatorSuiteSetup(void);
extern int gpggaSuiteSetup(void);
extern int gpgsaSuiteSetup(void);
extern int gpgsvSuiteSetup(void);
extern int gprmcSuiteSetup(void);
extern int gpvtgSuiteSetup(void);
extern int infoSuiteSetup(void);
extern int nmathSuiteSetup(void);
extern int parserSuiteSetup(void);
extern int sentenceSuiteSetup(void);
extern int utilSuiteSetup(void);
extern int validateSuiteSetup(void);

int main(void) {
  unsigned int failedCount = 0;

  if (CUE_SUCCESS != CU_initialize_registry()) {
    goto out;
  }

  if ( //
      (contextSuiteSetup() != CUE_SUCCESS) //
      || (generatorSuiteSetup() != CUE_SUCCESS) //
      || (gpggaSuiteSetup() != CUE_SUCCESS) //
      || (gpgsaSuiteSetup() != CUE_SUCCESS) //
      || (gpgsvSuiteSetup() != CUE_SUCCESS) //
      || (gprmcSuiteSetup() != CUE_SUCCESS) //
      || (gpvtgSuiteSetup() != CUE_SUCCESS) //
      || (infoSuiteSetup() != CUE_SUCCESS) //
      || (nmathSuiteSetup() != CUE_SUCCESS) //
      || (parserSuiteSetup() != CUE_SUCCESS) //
      || (sentenceSuiteSetup() != CUE_SUCCESS) //
      || (utilSuiteSetup() != CUE_SUCCESS) //
      || (validateSuiteSetup() != CUE_SUCCESS) //
      ) {
    goto cleanup;
  }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);

  CU_basic_run_tests();

  failedCount = CU_get_number_of_failures();

cleanup:
  CU_cleanup_registry();

out:
  if (failedCount) {
    exit(MIN(126,(int) failedCount));
  }

  return CU_get_error();
}

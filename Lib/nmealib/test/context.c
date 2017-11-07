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

#include <nmealib/context.h>
#include <nmealib/util.h>
#include <CUnit/Basic.h>
#include <stdlib.h>

/*
 * Forward declarations
 */

int contextSuiteSetup(void);

/*
 * Helpers
 */

static int nmeaTraceCalls = 0;
static int nmeaErrorCalls = 0;

static void traceFunction(const char *s __attribute__((unused)), size_t sz __attribute__((unused))) {
  nmeaTraceCalls++;
}

static void errorFunction(const char *s __attribute__((unused)), size_t sz __attribute__((unused))) {
  nmeaErrorCalls++;
}

static void reset(void) {
  nmeaTraceCalls = 0;
  nmeaErrorCalls = 0;
}

#define  validateContext(traces, errors) \
  { \
   CU_ASSERT_EQUAL(nmeaTraceCalls, traces); \
   CU_ASSERT_EQUAL(nmeaErrorCalls, errors); \
   reset(); \
  }

/*
 * Tests
 */

static void test_nmeaContextTrace(void) {
  const char * s = "some string";
  char * buf = NULL;
  NmeaContextPrintFunction prev = nmeaContextSetTraceFunction(NULL);

  CU_ASSERT_PTR_NOT_NULL(prev);

  reset();

  /* invalid inputs */

  nmeaContextTrace(NULL);
  validateContext(0, 0);

  /* no trace */

  nmeaContextTrace("%s", s);
  validateContext(0, 0);

  /* trace */

  nmeaContextSetTraceFunction(traceFunction);

  nmeaContextTrace("%s", s);
  validateContext(1, 0);

  /* empty string*/

  nmeaContextTrace("%s", "");
  validateContext(0, 0);

  /* very large string*/

  buf = malloc(2 * NMEALIB_BUFFER_CHUNK_SIZE);
  memset(buf, 'A', 2 * NMEALIB_BUFFER_CHUNK_SIZE);
  buf[(2 * NMEALIB_BUFFER_CHUNK_SIZE) - 1] = '\0';

  nmeaContextTrace("%s", buf);
  validateContext(1, 0);
  free(buf);
}

static void test_nmeaContextError(void) {
  const char * s = "some string";
  char * buf = NULL;
  NmeaContextPrintFunction prev = nmeaContextSetErrorFunction(NULL);

  CU_ASSERT_PTR_NOT_NULL(prev);

  reset();

  /* invalid inputs */

  nmeaContextError(NULL);
  validateContext(0, 0);

  /* no error */

  nmeaContextError("%s", s);
  validateContext(0, 0);

  /* error */

  nmeaContextSetErrorFunction(errorFunction);

  nmeaContextError("%s", s);
  validateContext(0, 1);

  /* empty string*/

  nmeaContextError("%s", "");
  validateContext(0, 0);

  /* very large string*/

  buf = malloc(2 * NMEALIB_BUFFER_CHUNK_SIZE);
  memset(buf, 'A', 2 * NMEALIB_BUFFER_CHUNK_SIZE);
  buf[(2 * NMEALIB_BUFFER_CHUNK_SIZE) - 1] = '\0';

  nmeaContextError("%s", buf);
  validateContext(0, 1);
  free(buf);
}

/*
 * Setup
 */

static int suiteInit(void) {
  NmeaContextPrintFunction prev;

  prev = nmeaContextSetTraceFunction(traceFunction);
  if (prev) {
    return CUE_SINIT_FAILED;
  }

  prev = nmeaContextSetErrorFunction(errorFunction);
  if (prev) {
    return CUE_SINIT_FAILED;
  }

  nmeaTraceCalls = 0;
  nmeaErrorCalls = 0;
  return CUE_SUCCESS;
}

static int suiteClean(void) {
  nmeaContextSetErrorFunction(NULL);
  nmeaContextSetTraceFunction(NULL);
  nmeaTraceCalls = 0;
  nmeaErrorCalls = 0;
  return CUE_SUCCESS;
}

int contextSuiteSetup(void) {
  CU_pSuite pSuite = CU_add_suite("context", suiteInit, suiteClean);
  if (!pSuite) {
    return CU_get_error();
  }

  if ( //
      (!CU_add_test(pSuite, "nmeaContextTrace", test_nmeaContextTrace)) //
      || (!CU_add_test(pSuite, "nmeaContextError", test_nmeaContextError)) //
      ) {
    return CU_get_error();
  }

  return CUE_SUCCESS;
}

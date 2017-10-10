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

#include "testHelpers.h"

#include <nmealib/validate.h>
#include <CUnit/Basic.h>

int validateSuiteSetup(void);

/*
 * Tests
 */

static void test_nmeaValidateIsInvalidCharacter(void) {
  const NmeaInvalidCharacter *r;

  r = nmeaValidateIsInvalidCharacter('A');
  CU_ASSERT_PTR_NULL(r);

  r = nmeaValidateIsInvalidCharacter(31);
  CU_ASSERT_EQUAL(r->character, '*');
  CU_ASSERT_STRING_EQUAL(r->description, "non-ASCII character");

  r = nmeaValidateIsInvalidCharacter(127);
  CU_ASSERT_EQUAL(r->character, '*');
  CU_ASSERT_STRING_EQUAL(r->description, "non-ASCII character");

  r = nmeaValidateIsInvalidCharacter('$');
  CU_ASSERT_EQUAL(r->character, '$');
  CU_ASSERT_STRING_EQUAL(r->description, "sentence delimiter");

  r = nmeaValidateIsInvalidCharacter('*');
  CU_ASSERT_EQUAL(r->character, '*');
  CU_ASSERT_STRING_EQUAL(r->description, "checksum field delimiter");

  r = nmeaValidateIsInvalidCharacter('!');
  CU_ASSERT_EQUAL(r->character, '!');
  CU_ASSERT_STRING_EQUAL(r->description, "exclamation mark");

  r = nmeaValidateIsInvalidCharacter('\\');
  CU_ASSERT_EQUAL(r->character, '\\');
  CU_ASSERT_STRING_EQUAL(r->description, "backslash");

  r = nmeaValidateIsInvalidCharacter('^');
  CU_ASSERT_EQUAL(r->character, '^');
  CU_ASSERT_STRING_EQUAL(r->description, "power");

  r = nmeaValidateIsInvalidCharacter('~');
  CU_ASSERT_EQUAL(r->character, '~');
  CU_ASSERT_STRING_EQUAL(r->description, "tilde");
}

static void test_nmeaValidateSentenceHasInvalidCharacters(void) {
  const NmeaInvalidCharacter *r;
  const char *s;

  r = nmeaValidateSentenceHasInvalidCharacters(NULL, 1);
  CU_ASSERT_PTR_NULL(r);

  s = "***";
  r = nmeaValidateSentenceHasInvalidCharacters(s, 0);
  CU_ASSERT_PTR_NULL(r);

  s = "dummy";
  r = nmeaValidateSentenceHasInvalidCharacters(s, strlen(s));
  CU_ASSERT_PTR_NULL(r);

  s = "invalid!";
  r = nmeaValidateSentenceHasInvalidCharacters(s, strlen(s));
  CU_ASSERT_EQUAL(r->character, '!');
  CU_ASSERT_STRING_EQUAL(r->description, "exclamation mark");
}

static void test_nmeaValidateTime(void) {
  bool r;
  const char *s = "dummy sentence";
  NmeaTime t;

  r = nmeaValidateTime(NULL, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);
  memset(&t, 0, sizeof(t));

  t.hour = 23;
  r = nmeaValidateTime(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);
  memset(&t, 0, sizeof(t));

  t.hour = 24;
  r = nmeaValidateTime(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&t, 0, sizeof(t));

  t.min = 59;
  r = nmeaValidateTime(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);
  memset(&t, 0, sizeof(t));

  t.min = 60;
  r = nmeaValidateTime(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&t, 0, sizeof(t));

  t.sec = 60;
  r = nmeaValidateTime(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);
  memset(&t, 0, sizeof(t));

  t.sec = 61;
  r = nmeaValidateTime(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&t, 0, sizeof(t));

  t.hsec = 99;
  r = nmeaValidateTime(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);
  memset(&t, 0, sizeof(t));

  t.hsec = 100;
  r = nmeaValidateTime(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&t, 0, sizeof(t));
}

static void test_nmeaValidateDate(void) {
  bool r;
  const char *s = "dummy sentence";
  NmeaTime t;

  r = nmeaValidateDate(NULL, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);
  memset(&t, 0, sizeof(t));

  t.year = 1899;
  t.mon = 1;
  t.day = 1;
  r = nmeaValidateDate(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&t, 0, sizeof(t));

  t.year = 2090;
  t.mon = 1;
  t.day = 1;
  r = nmeaValidateDate(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&t, 0, sizeof(t));

  t.year = 1900;
  t.mon = 0;
  t.day = 1;
  r = nmeaValidateDate(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&t, 0, sizeof(t));

  t.year = 1900;
  t.mon = 13;
  t.day = 1;
  r = nmeaValidateDate(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&t, 0, sizeof(t));

  t.year = 1900;
  t.mon = 1;
  t.day = 0;
  r = nmeaValidateDate(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&t, 0, sizeof(t));

  t.year = 1900;
  t.mon = 1;
  t.day = 32;
  r = nmeaValidateDate(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&t, 0, sizeof(t));

  t.year = 2016;
  t.mon = 7;
  t.day = 5;
  r = nmeaValidateDate(&t, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);
  memset(&t, 0, sizeof(t));
}

static void test_nmeaValidateNSEW(void) {
  bool r;
  const char *s = "dummy sentence";

  r = nmeaValidateNSEW('\0', true, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('\0', false, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('q', true, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('q', false, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('n', true, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('s', true, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('e', true, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('w', true, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('n', false, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('s', false, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('e', false, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('w', false, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('N', true, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateNSEW('S', true, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateNSEW('E', true, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('W', true, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('N', false, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('S', false, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateNSEW('E', false, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateNSEW('W', false, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);
}

static void test_nmeaValidateFix(void) {
  bool r;
  const char *s = "dummy sentence";

  r = nmeaValidateFix(NMEALIB_FIX_FIRST - 1, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateFix(NMEALIB_FIX_FIRST, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateFix(NMEALIB_FIX_LAST, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateFix(NMEALIB_FIX_LAST + 1, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
}

static void test_nmeaValidateSignal(void) {
  bool r;
  const char *s = "dummy sentence";

  r = nmeaValidateSignal(NMEALIB_SIG_FIRST - 1, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateSignal(NMEALIB_SIG_FIRST, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateSignal(NMEALIB_SIG_LAST, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateSignal(NMEALIB_SIG_LAST + 1, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
}

static void test_nmeaValidateMode(void) {
  bool r;
  const char *s = "dummy sentence";

  r = nmeaValidateMode('\0', "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);

  r = nmeaValidateMode('n', "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateMode('a', "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateMode('d', "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateMode('p', "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateMode('r', "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateMode('f', "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateMode('e', "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateMode('m', "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateMode('s', "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateMode('q', "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  r = nmeaValidateMode('N', "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateMode('A', "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateMode('D', "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateMode('P', "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateMode('R', "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateMode('F', "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateMode('E', "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateMode('M', "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateMode('S', "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);

  r = nmeaValidateMode('Q', "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
}

static void test_nmeaValidateSatellite(void) {
  bool r;
  const char *s = "dummy sentence";
  NmeaSatellite sat;

  r = nmeaValidateSatellite(NULL, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);
  memset(&sat, 0, sizeof(sat));

  sat.elevation = -181;
  r = nmeaValidateSatellite(&sat, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&sat, 0, sizeof(sat));

  sat.elevation = 181;
  r = nmeaValidateSatellite(&sat, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&sat, 0, sizeof(sat));

  sat.azimuth = 360;
  r = nmeaValidateSatellite(&sat, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&sat, 0, sizeof(sat));

  sat.snr = 100;
  r = nmeaValidateSatellite(&sat, "GPGGA", s);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
  memset(&sat, 0, sizeof(sat));

  r = nmeaValidateSatellite(&sat, "GPGGA", s);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);
  memset(&sat, 0, sizeof(sat));
}

/*
 * Setup
 */

int validateSuiteSetup(void) {
  CU_pSuite pSuite = CU_add_suite("validate", mockContextSuiteInit, mockContextSuiteClean);
  if (!pSuite) {
    return CU_get_error();
  }

  if ( //
      (!CU_add_test(pSuite, "nmeaValidateIsInvalidCharacter", test_nmeaValidateIsInvalidCharacter)) //
      || (!CU_add_test(pSuite, "nmeaValidateSentenceHasInvalidCharacters", test_nmeaValidateSentenceHasInvalidCharacters)) //
      || (!CU_add_test(pSuite, "nmeaValidateTime", test_nmeaValidateTime)) //
      || (!CU_add_test(pSuite, "nmeaValidateDate", test_nmeaValidateDate)) //
      || (!CU_add_test(pSuite, "nmeaValidateNSEW", test_nmeaValidateNSEW)) //
      || (!CU_add_test(pSuite, "nmeaValidateFix", test_nmeaValidateFix)) //
      || (!CU_add_test(pSuite, "nmeaValidateSignal", test_nmeaValidateSignal)) //
      || (!CU_add_test(pSuite, "nmeaValidateMode", test_nmeaValidateMode)) //
      || (!CU_add_test(pSuite, "nmeaValidateSatellite", test_nmeaValidateSatellite)) //
      ) {
    return CU_get_error();
  }

  return CUE_SUCCESS;
}

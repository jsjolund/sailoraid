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

#include <nmealib/util.h>
#include <CUnit/Basic.h>
#include <float.h>
#include <limits.h>

int utilSuiteSetup(void);

/*
 * Tests
 */

static void test_nmeaRandomInit(void) {
  nmeaRandomInit();
}

static void test_nmeaRandom(void) {
  double r;

  r = nmeaRandom(10.0, 20.0);
  CU_ASSERT_EQUAL(r >= 10.0, true);
  CU_ASSERT_EQUAL(r <= 20.0, true);
}

static void test_Min(void) {
  int r;

  r = MIN(1, 1);
  CU_ASSERT_EQUAL(r, 1);

  r = MIN(1, 2);
  CU_ASSERT_EQUAL(r, 1);

  r = MIN(2, 1);
  CU_ASSERT_EQUAL(r, 1);

  r = MIN(-1, -2);
  CU_ASSERT_EQUAL(r, -2);

  r = MIN(-2, -1);
  CU_ASSERT_EQUAL(r, -2);
}

static void test_Max(void) {
  int r;

  r = MAX(1, 1);
  CU_ASSERT_EQUAL(r, 1);

  r = MAX(1, 2);
  CU_ASSERT_EQUAL(r, 2);

  r = MAX(2, 1);
  CU_ASSERT_EQUAL(r, 2);

  r = MAX(-1, -2);
  CU_ASSERT_EQUAL(r, -1);

  r = MAX(-2, -1);
  CU_ASSERT_EQUAL(r, -1);
}

static void test_nmeaStringTrim(void) {
  const char *s = NULL;
  size_t sz;

  sz = nmeaStringTrim(NULL);
  CU_ASSERT_EQUAL(sz, 0);

  sz = nmeaStringTrim(&s);
  CU_ASSERT_EQUAL(sz, 0);

  s = "";
  sz = nmeaStringTrim(&s);
  CU_ASSERT_EQUAL(sz, 0);

  s = "    \t   ";
  sz = nmeaStringTrim(&s);
  CU_ASSERT_EQUAL(sz, 0);

  s = "123456";
  sz = nmeaStringTrim(&s);
  CU_ASSERT_EQUAL(sz, 6);

  s = "     \t  123456";
  sz = nmeaStringTrim(&s);
  CU_ASSERT_EQUAL(sz, 6);

  s = "123456   \t   ";
  sz = nmeaStringTrim(&s);
  CU_ASSERT_EQUAL(sz, 6);
}

static void test_nmeaStringContainsWhitespace(void) {
  const char *s = NULL;
  size_t sz = 0;
  bool r;

  r = nmeaStringContainsWhitespace(NULL, 1);
  CU_ASSERT_EQUAL(r, false);

  r = nmeaStringContainsWhitespace(s, sz);
  CU_ASSERT_EQUAL(r, false);

  s = "123456";
  r = nmeaStringContainsWhitespace(s, strlen(s));
  CU_ASSERT_EQUAL(r, false);

  s = "123     456";
  r = nmeaStringContainsWhitespace(s, strlen(s));
  CU_ASSERT_EQUAL(r, true);

  s = "123\t   456";
  r = nmeaStringContainsWhitespace(s, strlen(s));
  CU_ASSERT_EQUAL(r, true);
}

static void test_nmeaCalculateCRC(void) {
  unsigned int r;
  const char *s = "dummy sentence";

  /* invalid inputs */

  r = nmeaCalculateCRC(NULL, 1);
  CU_ASSERT_EQUAL(r, 0xff);

  r = nmeaCalculateCRC(s, 0);
  CU_ASSERT_EQUAL(r, 0xff);

  /* empty */

  r = nmeaCalculateCRC("", 0);
  CU_ASSERT_EQUAL(r, 0xff);

  /* normal */

  r = nmeaCalculateCRC(s, strlen(s));
  CU_ASSERT_EQUAL(r, 73);

  /* normal, with leading '$' */

  s = "$GPGGA,dummy sentence";
  r = nmeaCalculateCRC(s, strlen(s));
  CU_ASSERT_EQUAL(r, 51);
}

static void test_nmeaStringToInteger(void) {
  int r;
  const char *s = "  15  ";

  /* invalid inputs */

  r = nmeaStringToInteger(NULL, 1, 10);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToInteger(s, 0, 10);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToInteger(s, INT_MAX, 10);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToInteger(s, strlen(s), 0);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  /* out of range */

  s = "-2147483649";
  r = nmeaStringToInteger(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, INT_MIN);
  validateContext(0, 0);

  s = "2147483649";
  r = nmeaStringToInteger(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, INT_MAX);
  validateContext(0, 0);

  /* not a number */

  s = "  ";
  r = nmeaStringToInteger(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, INT_MAX);
  validateContext(0, 1);

  s = "  not a number ";
  r = nmeaStringToInteger(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, INT_MAX);
  validateContext(0, 1);

  /* number */

  s = "-15";
  r = nmeaStringToInteger(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, -15);
  validateContext(0, 0);

  s = "15";
  r = nmeaStringToInteger(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, 15);
  validateContext(0, 0);
}

static void test_nmeaStringToUnsignedInteger(void) {
  unsigned int r;
  const char *s = "  15  ";

  /* invalid inputs */

  r = nmeaStringToUnsignedInteger(NULL, 1, 10);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToUnsignedInteger(s, 0, 10);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToUnsignedInteger(s, UINT_MAX, 10);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToUnsignedInteger(s, strlen(s), 0);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  /* out of range */

  s = "4294967296";
  r = nmeaStringToUnsignedInteger(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, UINT_MAX);
  validateContext(0, 0);

  /* not a number */

  s = "  ";
  r = nmeaStringToUnsignedInteger(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, UINT_MAX);
  validateContext(0, 1);

  s = "  not a number ";
  r = nmeaStringToUnsignedInteger(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, UINT_MAX);
  validateContext(0, 1);

  /* number */

  s = "15";
  r = nmeaStringToUnsignedInteger(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, 15);
  validateContext(0, 0);
}

static void test_nmeaStringToLong(void) {
  long r;
  const char *s = "  15  ";

  /* invalid inputs */

  r = nmeaStringToLong(NULL, 1, 10);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToLong(s, 0, 10);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToLong(s, INT_MAX, 10);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToLong(s, strlen(s), 0);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  /* not a number */

  s = "  ";
  r = nmeaStringToLong(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, LONG_MAX);
  validateContext(0, 1);

  s = "  not a number ";
  r = nmeaStringToLong(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, LONG_MAX);
  validateContext(0, 1);

  /* number */

  s = "-15";
  r = nmeaStringToLong(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, -15);
  validateContext(0, 0);

  s = "15";
  r = nmeaStringToLong(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, 15);
  validateContext(0, 0);
}

static void test_nmeaStringToUnsignedLong(void) {
  unsigned long r;
  const char *s = "  15  ";

  /* invalid inputs */

  r = nmeaStringToUnsignedLong(NULL, 1, 10);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToUnsignedLong(s, 0, 10);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToUnsignedLong(s, INT_MAX, 10);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToUnsignedLong(s, strlen(s), 0);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  /* not a number */

  s = "  ";
  r = nmeaStringToUnsignedLong(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, ULONG_MAX);
  validateContext(0, 1);

  s = "  not a number ";
  r = nmeaStringToUnsignedLong(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, ULONG_MAX);
  validateContext(0, 1);

  /* number */

  s = "15";
  r = nmeaStringToUnsignedLong(s, strlen(s), 10);
  CU_ASSERT_EQUAL(r, 15);
  validateContext(0, 0);
}

static void test_nmeaStringToDouble(void) {
  double r;
  const char *s = "  15.42  ";

  /* invalid inputs */

  r = nmeaStringToDouble(NULL, 1);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToDouble(s, 0);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  r = nmeaStringToDouble(s, INT_MAX);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);

  /* not a number */

  s = "  ";
  r = nmeaStringToDouble(s, strlen(s));
  CU_ASSERT_EQUAL(isNaN(r), true);
  validateContext(0, 1);

  s = "  not a number ";
  r = nmeaStringToDouble(s, strlen(s));
  CU_ASSERT_EQUAL(isNaN(r), true);
  validateContext(0, 1);

  /* number */

  s = "15";
  r = nmeaStringToDouble(s, strlen(s));
  CU_ASSERT_EQUAL(r, 15);
  validateContext(0, 0);
}

static void test_nmeaAppendChecksum(void) {
  int r;
  char s[32] = "dummy sentence";

  r = nmeaAppendChecksum(NULL, 1, 1);
  CU_ASSERT_EQUAL(r, 0);

  strncpy(s, "dummy sentence", sizeof(s));
  r = nmeaAppendChecksum(s, sizeof(s), strlen(s));
  CU_ASSERT_EQUAL(r, 5);
  CU_ASSERT_STRING_EQUAL(s, "dummy sentence*49\r\n");

  strncpy(s, "dummy sentence", sizeof(s));
  r = nmeaAppendChecksum(s, 0, strlen(s));
  CU_ASSERT_EQUAL(r, 5);
  CU_ASSERT_STRING_EQUAL(s, "dummy sentence");

  strncpy(s, "dummy sentence", sizeof(s));
  r = nmeaAppendChecksum(s, strlen(s), strlen(s));
  CU_ASSERT_EQUAL(r, 5);
  CU_ASSERT_STRING_EQUAL(s, "dummy sentence");

  strncpy(s, "dummy sentence", sizeof(s));
  r = nmeaAppendChecksum(s, strlen(s) + 1, strlen(s));
  CU_ASSERT_EQUAL(r, 5);
  CU_ASSERT_STRING_EQUAL(s, "dummy sentence");

  strncpy(s, "dummy sentence", sizeof(s));
  r = nmeaAppendChecksum(s, strlen(s) + 2, strlen(s));
  CU_ASSERT_EQUAL(r, 5);
  CU_ASSERT_STRING_EQUAL(s, "dummy sentence*");

  strncpy(s, "dummy sentence", sizeof(s));
  r = nmeaAppendChecksum(s, strlen(s) + 3, strlen(s));
  CU_ASSERT_EQUAL(r, 5);
  CU_ASSERT_STRING_EQUAL(s, "dummy sentence*4");

  strncpy(s, "dummy sentence", sizeof(s));
  r = nmeaAppendChecksum(s, strlen(s) + 4, strlen(s));
  CU_ASSERT_EQUAL(r, 5);
  CU_ASSERT_STRING_EQUAL(s, "dummy sentence*49");

  strncpy(s, "dummy sentence", sizeof(s));
  r = nmeaAppendChecksum(s, strlen(s) + 5, strlen(s));
  CU_ASSERT_EQUAL(r, 5);
  CU_ASSERT_STRING_EQUAL(s, "dummy sentence*49\r");

  strncpy(s, "dummy sentence", sizeof(s));
  r = nmeaAppendChecksum(s, strlen(s) + 6, strlen(s));
  CU_ASSERT_EQUAL(r, 5);
  CU_ASSERT_STRING_EQUAL(s, "dummy sentence*49\r\n");

  strncpy(s, "dummy sentence", sizeof(s));
  r = nmeaAppendChecksum(s, strlen(s) + 7, strlen(s));
  CU_ASSERT_EQUAL(r, 5);
  CU_ASSERT_STRING_EQUAL(s, "dummy sentence*49\r\n");
}

static void test_nmeaPrintf(void) {
  int r;
  char buf[128];
  char s[32] = "dummy sentence";

  memset(buf, 0, sizeof(buf));

  r = nmeaPrintf(NULL, 1, "%s", s);
  CU_ASSERT_EQUAL(r, 0);
  memset(buf, 0, sizeof(buf));

  r = nmeaPrintf(buf, 1, NULL);
  CU_ASSERT_EQUAL(r, 0);
  memset(buf, 0, sizeof(buf));

  r = nmeaPrintf(buf, 0, "%s", s);
  CU_ASSERT_EQUAL(r, 19);
  memset(buf, 0, sizeof(buf));

  r = nmeaPrintf(buf, 15, "%s", s);
  CU_ASSERT_EQUAL(r, 19);
  CU_ASSERT_STRING_EQUAL(buf, "dummy sentence");
  memset(buf, 0, sizeof(buf));

  r = nmeaPrintf(buf, 16, "%s", s);
  CU_ASSERT_EQUAL(r, 19);
  CU_ASSERT_STRING_EQUAL(buf, "dummy sentence*");
  memset(buf, 0, sizeof(buf));

  r = nmeaPrintf(buf, 17, "%s", s);
  CU_ASSERT_EQUAL(r, 19);
  CU_ASSERT_STRING_EQUAL(buf, "dummy sentence*4");
  memset(buf, 0, sizeof(buf));

  r = nmeaPrintf(buf, 18, "%s", s);
  CU_ASSERT_EQUAL(r, 19);
  CU_ASSERT_STRING_EQUAL(buf, "dummy sentence*49");
  memset(buf, 0, sizeof(buf));

  r = nmeaPrintf(buf, 19, "%s", s);
  CU_ASSERT_EQUAL(r, 19);
  CU_ASSERT_STRING_EQUAL(buf, "dummy sentence*49\r");
  memset(buf, 0, sizeof(buf));

  r = nmeaPrintf(buf, 20, "%s", s);
  CU_ASSERT_EQUAL(r, 19);
  CU_ASSERT_STRING_EQUAL(buf, "dummy sentence*49\r\n");
  memset(buf, 0, sizeof(buf));

  r = nmeaPrintf(buf, sizeof(buf), "%s", s);
  CU_ASSERT_EQUAL(r, 19);
  CU_ASSERT_STRING_EQUAL(buf, "dummy sentence*49\r\n");
  memset(buf, 0, sizeof(buf));
}

static void test_nmeaScanf(void) {
  const char *s;
  char s1[32];
  char s2[32];
  char s3[32];
  double f1;
  double f2;
  double f3;
  long l1;
  long l2;
  long l3;
  int d1;
  int d2;
  int d3;
  unsigned int u1;
  unsigned int u2;
  unsigned int u3;
  size_t r;

  /* invalid inputs */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(NULL, strlen(s), "%s,%s, %s", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), NULL, s1, s2, s3);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  /* no match on regular character */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), "_GPGGA,%s,%s", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  /*
   * String
   */

  /* filled fields */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), "%s,%s,%s", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$GPGGA");
  CU_ASSERT_STRING_EQUAL(s2, "something");
  CU_ASSERT_STRING_EQUAL(s3, "else");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), "%s,%s,%s*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$GPGGA");
  CU_ASSERT_STRING_EQUAL(s2, "something");
  CU_ASSERT_STRING_EQUAL(s3, "else");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else*11";
  r = nmeaScanf(s, strlen(s), "%s,%s,%s*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$GPGGA");
  CU_ASSERT_STRING_EQUAL(s2, "something");
  CU_ASSERT_STRING_EQUAL(s3, "else");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else*11\r\n";
  r = nmeaScanf(s, strlen(s), "%s,%s,%s*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$GPGGA");
  CU_ASSERT_STRING_EQUAL(s2, "something");
  CU_ASSERT_STRING_EQUAL(s3, "else");

  /* empty fields */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%s,%s,%s", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%s,%s,%s*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,*11";
  r = nmeaScanf(s, strlen(s), "%s,%s,%s*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,*11\r\n";
  r = nmeaScanf(s, strlen(s), "%s,%s,%s*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  /* size larger than fields */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), "%32s,%32s,%32s", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$GPGGA");
  CU_ASSERT_STRING_EQUAL(s2, "something");
  CU_ASSERT_STRING_EQUAL(s3, "else");

  /* fields larger than size */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,1234567890123456789012345678901234567890,else";
  r = nmeaScanf(s, strlen(s), "%32s,%32s,%32s", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$GPGGA");
  CU_ASSERT_STRING_EQUAL(s2, "1234567890123456789012345678901");
  CU_ASSERT_STRING_EQUAL(s3, "else");

  /* size larger than empty fields */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%32s,%32s,%32s", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  /* size zero */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), "%0s,%0s,%0s", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  /*
   * Char (as-is)
   */

  /* filled fields */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), "%c,%c,%c", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$");
  CU_ASSERT_STRING_EQUAL(s2, "s");
  CU_ASSERT_STRING_EQUAL(s3, "e");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), "%c,%c,%c*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$");
  CU_ASSERT_STRING_EQUAL(s2, "s");
  CU_ASSERT_STRING_EQUAL(s3, "e");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else*11";
  r = nmeaScanf(s, strlen(s), "%c,%c,%c*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$");
  CU_ASSERT_STRING_EQUAL(s2, "s");
  CU_ASSERT_STRING_EQUAL(s3, "e");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,Something,else*11\r\n";
  r = nmeaScanf(s, strlen(s), "%c,%c,%c*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$");
  CU_ASSERT_STRING_EQUAL(s2, "S");
  CU_ASSERT_STRING_EQUAL(s3, "e");

  /* empty fields */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%c,%c,%c", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%c,%c,%c*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,*11";
  r = nmeaScanf(s, strlen(s), "%c,%c,%c*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,*11\r\n";
  r = nmeaScanf(s, strlen(s), "%c,%c,%c*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  /* size larger than fields */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), "%32c,%32c,%32c", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$");
  CU_ASSERT_STRING_EQUAL(s2, "s");
  CU_ASSERT_STRING_EQUAL(s3, "e");

  /* fields larger than size */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,1234567890123456789012345678901234567890,else";
  r = nmeaScanf(s, strlen(s), "%32c,%32c,%32c", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$");
  CU_ASSERT_STRING_EQUAL(s2, "1");
  CU_ASSERT_STRING_EQUAL(s3, "e");

  /* size larger than empty fields */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%32c,%32c,%32c", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  /* size zero */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), "%0c,%0c,%0c", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  /*
   * Char (upper-case)
   */

  /* filled fields */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), "%C,%C,%C", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$");
  CU_ASSERT_STRING_EQUAL(s2, "S");
  CU_ASSERT_STRING_EQUAL(s3, "E");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), "%C,%C,%C*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$");
  CU_ASSERT_STRING_EQUAL(s2, "S");
  CU_ASSERT_STRING_EQUAL(s3, "E");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else*11";
  r = nmeaScanf(s, strlen(s), "%C,%C,%C*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$");
  CU_ASSERT_STRING_EQUAL(s2, "S");
  CU_ASSERT_STRING_EQUAL(s3, "E");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,Something,else*11\r\n";
  r = nmeaScanf(s, strlen(s), "%C,%C,%C*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$");
  CU_ASSERT_STRING_EQUAL(s2, "S");
  CU_ASSERT_STRING_EQUAL(s3, "E");

  /* empty fields */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%C,%C,%C", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%C,%C,%C*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,*11";
  r = nmeaScanf(s, strlen(s), "%C,%C,%C*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,*11\r\n";
  r = nmeaScanf(s, strlen(s), "%C,%C,%C*", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  /* size larger than fields */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), "%32C,%32C,%32C", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$");
  CU_ASSERT_STRING_EQUAL(s2, "S");
  CU_ASSERT_STRING_EQUAL(s3, "E");

  /* fields larger than size */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,1234567890123456789012345678901234567890,else";
  r = nmeaScanf(s, strlen(s), "%32C,%32C,%32C", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "$");
  CU_ASSERT_STRING_EQUAL(s2, "1");
  CU_ASSERT_STRING_EQUAL(s3, "E");

  /* size larger than empty fields */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%32C,%32C,%32C", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  /* size zero */

  memset(s1, 0, sizeof(s1));
  memset(s2, 0, sizeof(s2));
  memset(s3, 0, sizeof(s3));
  s = "$GPGGA,something,else";
  r = nmeaScanf(s, strlen(s), "%0C,%0C,%0C", s1, s2, s3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_STRING_EQUAL(s1, "");
  CU_ASSERT_STRING_EQUAL(s2, "");
  CU_ASSERT_STRING_EQUAL(s3, "");

  /*
   * Double (as-is)
   */

  /* invalid */

  f1 = f2 = f3 = INFINITY;
  s = "$GPGGA,-1.2,42.5";
  r = nmeaScanf(s, strlen(s), "%f,%f,%f", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 1);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  /* filled fields */

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.2,42.5";
  r = nmeaScanf(s, strlen(s), "%f,%f,%f", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_DOUBLE_EQUAL(f1, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f2, -1.2, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f3, 42.5, FLT_EPSILON);

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.2,42.5";
  r = nmeaScanf(s, strlen(s), "%f,%f,%f*", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_DOUBLE_EQUAL(f1, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f2, -1.2, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f3, 42.5, FLT_EPSILON);

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.2,42.5*11";
  r = nmeaScanf(s, strlen(s), "%f,%f,%f*", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_DOUBLE_EQUAL(f1, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f2, -1.2, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f3, 42.5, FLT_EPSILON);

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.2,42.5*11\r\n";
  r = nmeaScanf(s, strlen(s), "%f,%f,%f*", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_DOUBLE_EQUAL(f1, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f2, -1.2, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f3, 42.5, FLT_EPSILON);

  /* empty fields */

  f1 = f2 = f3 = INFINITY;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%f,%f,%f", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  f1 = f2 = f3 = INFINITY;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%f,%f,%f*", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  f1 = f2 = f3 = INFINITY;
  s = ",,*11";
  r = nmeaScanf(s, strlen(s), "%f,%f,%f*", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  f1 = f2 = f3 = INFINITY;
  s = ",,*11\r\n";
  r = nmeaScanf(s, strlen(s), "%f,%f,%f*", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  /* size larger than fields */

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.2,42.5";
  r = nmeaScanf(s, strlen(s), "%32f,%32f,%32f", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 1);
  validateContext(0, 0);
  CU_ASSERT_DOUBLE_EQUAL(f1, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  /* fields larger than size */

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.21234543,42.5";
  r = nmeaScanf(s, strlen(s), "%f,%6f,%f", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 2);
  validateContext(0, 0);
  CU_ASSERT_DOUBLE_EQUAL(f1, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f2, -1.212, FLT_EPSILON);
  CU_ASSERT_EQUAL(isinf(f3), true);

  /* size larger than empty fields */

  f1 = f2 = f3 = INFINITY;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%32f,%32f,%32f", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 1);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  /* size zero */

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.2,42.5";
  r = nmeaScanf(s, strlen(s), "%0f,%0f,%0f", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  /*
   * Double (absolute)
   */

  /* invalid */

  f1 = f2 = f3 = INFINITY;
  s = "$GPGGA,-1.2,42.5";
  r = nmeaScanf(s, strlen(s), "%F,%F,%F", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 1);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  /* filled fields */

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.2,42.5";
  r = nmeaScanf(s, strlen(s), "%F,%F,%F", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_DOUBLE_EQUAL(f1, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f2, 1.2, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f3, 42.5, FLT_EPSILON);

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.2,42.5";
  r = nmeaScanf(s, strlen(s), "%F,%F,%F*", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_DOUBLE_EQUAL(f1, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f2, 1.2, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f3, 42.5, FLT_EPSILON);

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.2,42.5*11";
  r = nmeaScanf(s, strlen(s), "%F,%F,%F*", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_DOUBLE_EQUAL(f1, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f2, 1.2, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f3, 42.5, FLT_EPSILON);

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.2,42.5*11\r\n";
  r = nmeaScanf(s, strlen(s), "%F,%F,%F*", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_DOUBLE_EQUAL(f1, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f2, 1.2, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f3, 42.5, FLT_EPSILON);

  /* empty fields */

  f1 = f2 = f3 = INFINITY;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%F,%F,%F", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  f1 = f2 = f3 = INFINITY;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%F,%F,%F*", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  f1 = f2 = f3 = INFINITY;
  s = ",,*11";
  r = nmeaScanf(s, strlen(s), "%F,%F,%F*", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  f1 = f2 = f3 = INFINITY;
  s = ",,*11\r\n";
  r = nmeaScanf(s, strlen(s), "%F,%F,%F*", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  /* size larger than fields */

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.2,42.5";
  r = nmeaScanf(s, strlen(s), "%32F,%32F,%32F", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 1);
  validateContext(0, 0);
  CU_ASSERT_DOUBLE_EQUAL(f1, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  /* fields larger than size */

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.21234543,42.5";
  r = nmeaScanf(s, strlen(s), "%F,%6F,%F", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 2);
  validateContext(0, 0);
  CU_ASSERT_DOUBLE_EQUAL(f1, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(f2, 1.212, FLT_EPSILON);
  CU_ASSERT_EQUAL(isinf(f3), true);

  /* size larger than empty fields */

  f1 = f2 = f3 = INFINITY;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%32F,%32F,%32F", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 1);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  /* size zero */

  f1 = f2 = f3 = INFINITY;
  s = "0.0,-1.2,42.5";
  r = nmeaScanf(s, strlen(s), "%0F,%0F,%0F", &f1, &f2, &f3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(isinf(f1), true);
  CU_ASSERT_EQUAL(isinf(f2), true);
  CU_ASSERT_EQUAL(isinf(f3), true);

  /*
   * Long
   */

  /* invalid */

  l1 = l2 = l3 = LONG_MAX;
  s = "$GPGGA,-12,42";
  r = nmeaScanf(s, strlen(s), "%l,%l,%l", &l1, &l2, &l3);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 1);
  CU_ASSERT_EQUAL(l1, LONG_MAX);
  CU_ASSERT_EQUAL(l2, LONG_MAX);
  CU_ASSERT_EQUAL(l3, LONG_MAX);

  /* filled fields */

  l1 = l2 = l3 = LONG_MAX;
  s = "0,-12,42";
  r = nmeaScanf(s, strlen(s), "%l,%l,%l", &l1, &l2, &l3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(l1, 0);
  CU_ASSERT_EQUAL(l2, -12);
  CU_ASSERT_EQUAL(l3, 42);

  l1 = l2 = l3 = LONG_MAX;
  s = "0,-12,42";
  r = nmeaScanf(s, strlen(s), "%l,%l,%l*", &l1, &l2, &l3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(l1, 0);
  CU_ASSERT_EQUAL(l2, -12);
  CU_ASSERT_EQUAL(l3, 42);

  l1 = l2 = l3 = LONG_MAX;
  s = "0,-12,42*11";
  r = nmeaScanf(s, strlen(s), "%l,%l,%l*", &l1, &l2, &l3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(l1, 0);
  CU_ASSERT_EQUAL(l2, -12);
  CU_ASSERT_EQUAL(l3, 42);

  l1 = l2 = l3 = LONG_MAX;
  s = "0,-12,42*11\r\n";
  r = nmeaScanf(s, strlen(s), "%l,%l,%l*", &l1, &l2, &l3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(l1, 0);
  CU_ASSERT_EQUAL(l2, -12);
  CU_ASSERT_EQUAL(l3, 42);

  /* empty fields */

  l1 = l2 = l3 = LONG_MAX;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%l,%l,%l", &l1, &l2, &l3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(l1, LONG_MAX);
  CU_ASSERT_EQUAL(l2, LONG_MAX);
  CU_ASSERT_EQUAL(l3, LONG_MAX);

  l1 = l2 = l3 = LONG_MAX;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%l,%l,%l*", &l1, &l2, &l3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(l1, LONG_MAX);
  CU_ASSERT_EQUAL(l2, LONG_MAX);
  CU_ASSERT_EQUAL(l3, LONG_MAX);

  l1 = l2 = l3 = LONG_MAX;
  s = ",,*11";
  r = nmeaScanf(s, strlen(s), "%l,%l,%l*", &l1, &l2, &l3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(l1, LONG_MAX);
  CU_ASSERT_EQUAL(l2, LONG_MAX);
  CU_ASSERT_EQUAL(l3, LONG_MAX);

  l1 = l2 = l3 = LONG_MAX;
  s = ",,*11\r\n";
  r = nmeaScanf(s, strlen(s), "%l,%l,%l*", &l1, &l2, &l3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(l1, LONG_MAX);
  CU_ASSERT_EQUAL(l2, LONG_MAX);
  CU_ASSERT_EQUAL(l3, LONG_MAX);

  /* size larger than fields */

  l1 = l2 = l3 = LONG_MAX;
  s = "0,-12,42";
  r = nmeaScanf(s, strlen(s), "%32l,%32l,%32l", &l1, &l2, &l3);
  CU_ASSERT_EQUAL(r, 1);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(l1, 0);
  CU_ASSERT_EQUAL(l2, LONG_MAX);
  CU_ASSERT_EQUAL(l3, LONG_MAX);

  /* fields larger than size */

  l1 = l2 = l3 = LONG_MAX;
  s = "0,-121234,42";
  r = nmeaScanf(s, strlen(s), "%l,%6l,%l", &l1, &l2, &l3);
  CU_ASSERT_EQUAL(r, 2);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(l1, 0);
  CU_ASSERT_EQUAL(l2, -12123);
  CU_ASSERT_EQUAL(l3, LONG_MAX);

  /* size larger than empty fields */

  l1 = l2 = l3 = LONG_MAX;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%32l,%32l,%32l", &l1, &l2, &l3);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 1);
  CU_ASSERT_EQUAL(l1, LONG_MAX);
  CU_ASSERT_EQUAL(l2, LONG_MAX);
  CU_ASSERT_EQUAL(l3, LONG_MAX);

  /* size zero */

  l1 = l2 = l3 = LONG_MAX;
  s = "0,-12,42";
  r = nmeaScanf(s, strlen(s), "%0l,%0l,%0l", &l1, &l2, &l3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(l1, LONG_MAX);
  CU_ASSERT_EQUAL(l2, LONG_MAX);
  CU_ASSERT_EQUAL(l3, LONG_MAX);

  /*
   * Integer
   */

  /* invalid */

  d1 = d2 = d3 = INT_MAX;
  s = "$GPGGA,-12,42";
  r = nmeaScanf(s, strlen(s), "%d,%d,%d", &d1, &d2, &d3);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 1);
  CU_ASSERT_EQUAL(d1, INT_MAX);
  CU_ASSERT_EQUAL(d2, INT_MAX);
  CU_ASSERT_EQUAL(d3, INT_MAX);

  /* filled fields */

  d1 = d2 = d3 = INT_MAX;
  s = "0,-12,42";
  r = nmeaScanf(s, strlen(s), "%d,%d,%d", &d1, &d2, &d3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(d1, 0);
  CU_ASSERT_EQUAL(d2, -12);
  CU_ASSERT_EQUAL(d3, 42);

  d1 = d2 = d3 = INT_MAX;
  s = "0,-12,42";
  r = nmeaScanf(s, strlen(s), "%d,%d,%d*", &d1, &d2, &d3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(d1, 0);
  CU_ASSERT_EQUAL(d2, -12);
  CU_ASSERT_EQUAL(d3, 42);

  d1 = d2 = d3 = INT_MAX;
  s = "0,-12,42*11";
  r = nmeaScanf(s, strlen(s), "%d,%d,%d*", &d1, &d2, &d3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(d1, 0);
  CU_ASSERT_EQUAL(d2, -12);
  CU_ASSERT_EQUAL(d3, 42);

  d1 = d2 = d3 = INT_MAX;
  s = "0,-12,42*11\r\n";
  r = nmeaScanf(s, strlen(s), "%d,%d,%d*", &d1, &d2, &d3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(d1, 0);
  CU_ASSERT_EQUAL(d2, -12);
  CU_ASSERT_EQUAL(d3, 42);

  /* empty fields */

  d1 = d2 = d3 = INT_MAX;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%d,%d,%d", &d1, &d2, &d3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(d1, INT_MAX);
  CU_ASSERT_EQUAL(d2, INT_MAX);
  CU_ASSERT_EQUAL(d3, INT_MAX);

  d1 = d2 = d3 = INT_MAX;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%d,%d,%d*", &d1, &d2, &d3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(d1, INT_MAX);
  CU_ASSERT_EQUAL(d2, INT_MAX);
  CU_ASSERT_EQUAL(d3, INT_MAX);

  d1 = d2 = d3 = INT_MAX;
  s = ",,*11";
  r = nmeaScanf(s, strlen(s), "%d,%d,%d*", &d1, &d2, &d3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(d1, INT_MAX);
  CU_ASSERT_EQUAL(d2, INT_MAX);
  CU_ASSERT_EQUAL(d3, INT_MAX);

  d1 = d2 = d3 = INT_MAX;
  s = ",,*11\r\n";
  r = nmeaScanf(s, strlen(s), "%d,%d,%d*", &d1, &d2, &d3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(d1, INT_MAX);
  CU_ASSERT_EQUAL(d2, INT_MAX);
  CU_ASSERT_EQUAL(d3, INT_MAX);

  /* size larger than fields */

  d1 = d2 = d3 = INT_MAX;
  s = "0,-12,42";
  r = nmeaScanf(s, strlen(s), "%32d,%32d,%32d", &d1, &d2, &d3);
  CU_ASSERT_EQUAL(r, 1);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(d1, 0);
  CU_ASSERT_EQUAL(d2, INT_MAX);
  CU_ASSERT_EQUAL(d3, INT_MAX);

  /* fields larger than size */

  d1 = d2 = d3 = INT_MAX;
  s = "0,-121234,42";
  r = nmeaScanf(s, strlen(s), "%d,%6d,%d", &d1, &d2, &d3);
  CU_ASSERT_EQUAL(r, 2);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(d1, 0);
  CU_ASSERT_EQUAL(d2, -12123);
  CU_ASSERT_EQUAL(d3, INT_MAX);

  /* size larger than empty fields */

  d1 = d2 = d3 = INT_MAX;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%32d,%32d,%32d", &d1, &d2, &d3);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 1);
  CU_ASSERT_EQUAL(d1, INT_MAX);
  CU_ASSERT_EQUAL(d2, INT_MAX);
  CU_ASSERT_EQUAL(d3, INT_MAX);

  /* size zero */

  d1 = d2 = d3 = INT_MAX;
  s = "0,-12,42";
  r = nmeaScanf(s, strlen(s), "%0d,%0d,%0d", &d1, &d2, &d3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(d1, INT_MAX);
  CU_ASSERT_EQUAL(d2, INT_MAX);
  CU_ASSERT_EQUAL(d3, INT_MAX);

  /*
   * Unsigned Integer
   */

  /* invalid */

  u1 = u2 = u3 = UINT_MAX;
  s = "$GPGGA,12,42";
  r = nmeaScanf(s, strlen(s), "%u,%u,%u", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 1);
  CU_ASSERT_EQUAL(u1, UINT_MAX);
  CU_ASSERT_EQUAL(u2, UINT_MAX);
  CU_ASSERT_EQUAL(u3, UINT_MAX);

  /* filled fields */

  u1 = u2 = u3 = UINT_MAX;
  s = "0,12,42";
  r = nmeaScanf(s, strlen(s), "%u,%u,%u", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(u1, 0);
  CU_ASSERT_EQUAL(u2, 12);
  CU_ASSERT_EQUAL(u3, 42);

  u1 = u2 = u3 = UINT_MAX;
  s = "0,12,42";
  r = nmeaScanf(s, strlen(s), "%u,%u,%u*", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(u1, 0);
  CU_ASSERT_EQUAL(u2, 12);
  CU_ASSERT_EQUAL(u3, 42);

  u1 = u2 = u3 = UINT_MAX;
  s = "0,12,42*11";
  r = nmeaScanf(s, strlen(s), "%u,%u,%u*", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(u1, 0);
  CU_ASSERT_EQUAL(u2, 12);
  CU_ASSERT_EQUAL(u3, 42);

  u1 = u2 = u3 = UINT_MAX;
  s = "0,12,42*11\r\n";
  r = nmeaScanf(s, strlen(s), "%u,%u,%u*", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(u1, 0);
  CU_ASSERT_EQUAL(u2, 12);
  CU_ASSERT_EQUAL(u3, 42);

  /* empty fields */

  u1 = u2 = u3 = UINT_MAX;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%u,%u,%u", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(u1, UINT_MAX);
  CU_ASSERT_EQUAL(u2, UINT_MAX);
  CU_ASSERT_EQUAL(u3, UINT_MAX);

  u1 = u2 = u3 = UINT_MAX;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%u,%u,%u*", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(u1, UINT_MAX);
  CU_ASSERT_EQUAL(u2, UINT_MAX);
  CU_ASSERT_EQUAL(u3, UINT_MAX);

  u1 = u2 = u3 = UINT_MAX;
  s = ",,*11";
  r = nmeaScanf(s, strlen(s), "%u,%u,%u*", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(u1, UINT_MAX);
  CU_ASSERT_EQUAL(u2, UINT_MAX);
  CU_ASSERT_EQUAL(u3, UINT_MAX);

  u1 = u2 = u3 = UINT_MAX;
  s = ",,*11\r\n";
  r = nmeaScanf(s, strlen(s), "%u,%u,%u*", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(u1, UINT_MAX);
  CU_ASSERT_EQUAL(u2, UINT_MAX);
  CU_ASSERT_EQUAL(u3, UINT_MAX);

  /* size larger than fields */

  u1 = u2 = u3 = UINT_MAX;
  s = "0,12,42";
  r = nmeaScanf(s, strlen(s), "%32u,%32u,%32u", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 1);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(u1, 0);
  CU_ASSERT_EQUAL(u2, UINT_MAX);
  CU_ASSERT_EQUAL(u3, UINT_MAX);

  /* fields larger than size */

  u1 = u2 = u3 = UINT_MAX;
  s = "0,121234,42";
  r = nmeaScanf(s, strlen(s), "%u,%5u,%u", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 2);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(u1, 0);
  CU_ASSERT_EQUAL(u2, 12123);
  CU_ASSERT_EQUAL(u3, UINT_MAX);

  /* size larger than empty fields */

  u1 = u2 = u3 = UINT_MAX;
  s = ",,";
  r = nmeaScanf(s, strlen(s), "%32u,%32u,%32u", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 1);
  CU_ASSERT_EQUAL(u1, UINT_MAX);
  CU_ASSERT_EQUAL(u2, UINT_MAX);
  CU_ASSERT_EQUAL(u3, UINT_MAX);

  /* size zero */

  u1 = u2 = u3 = UINT_MAX;
  s = "0,12,42";
  r = nmeaScanf(s, strlen(s), "%0d,%0d,%0d", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 3);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(u1, UINT_MAX);
  CU_ASSERT_EQUAL(u2, UINT_MAX);
  CU_ASSERT_EQUAL(u3, UINT_MAX);

  /*
   * Wrong format
   */

  u1 = u2 = u3 = UINT_MAX;
  s = "10,12,42";
  r = nmeaScanf(s, strlen(s), "%u,%^,%u", &u1, &u2, &u3);
  CU_ASSERT_EQUAL(r, 1);
  validateContext(0, 1);
  CU_ASSERT_EQUAL(u1, 10);
  CU_ASSERT_EQUAL(u2, UINT_MAX);
  CU_ASSERT_EQUAL(u3, UINT_MAX);
}

/*
 * Setup
 */

int utilSuiteSetup(void) {
  CU_pSuite pSuite = CU_add_suite("util", mockContextSuiteInit, mockContextSuiteClean);
  if (!pSuite) {
    return CU_get_error();
  }

  if ( //
      (!CU_add_test(pSuite, "nmeaInitRandom", test_nmeaRandomInit)) //
      || (!CU_add_test(pSuite, "nmeaRandom", test_nmeaRandom)) //
      || (!CU_add_test(pSuite, "MIN", test_Min)) //
      || (!CU_add_test(pSuite, "MAX", test_Max)) //
      || (!CU_add_test(pSuite, "nmeaStringTrim", test_nmeaStringTrim)) //
      || (!CU_add_test(pSuite, "nmeaStringContainsWhitespace", test_nmeaStringContainsWhitespace)) //
      || (!CU_add_test(pSuite, "nmeaCalculateCRC", test_nmeaCalculateCRC)) //
      || (!CU_add_test(pSuite, "nmeaStringToInteger", test_nmeaStringToInteger)) //
      || (!CU_add_test(pSuite, "nmeaStringToUnsignedInteger", test_nmeaStringToUnsignedInteger)) //
      || (!CU_add_test(pSuite, "nmeaStringToLong", test_nmeaStringToLong)) //
      || (!CU_add_test(pSuite, "nmeaStringToUnsignedLong", test_nmeaStringToUnsignedLong)) //
      || (!CU_add_test(pSuite, "nmeaStringToDouble", test_nmeaStringToDouble)) //
      || (!CU_add_test(pSuite, "nmeaAppendChecksum", test_nmeaAppendChecksum)) //
      || (!CU_add_test(pSuite, "nmeaPrintf", test_nmeaPrintf)) //
      || (!CU_add_test(pSuite, "nmeaScanf", test_nmeaScanf)) //
      ) {
    return CU_get_error();
  }

  return CUE_SUCCESS;
}

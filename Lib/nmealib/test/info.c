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

#include <nmealib/info.h>
#include <nmealib/nmath.h>
#include <nmealib/sentence.h>
#include <CUnit/Basic.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int infoSuiteSetup(void);

/*
 * Tests
 */

static void test_nmeaInfoSigToString(void) {
  const char * r;

  r = nmeaInfoSignalToString(NMEALIB_SIG_INVALID);
  CU_ASSERT_STRING_EQUAL(r, "INVALID");

  r = nmeaInfoSignalToString(NMEALIB_SIG_FIX);
  CU_ASSERT_STRING_EQUAL(r, "FIX");

  r = nmeaInfoSignalToString(NMEALIB_SIG_DIFFERENTIAL);
  CU_ASSERT_STRING_EQUAL(r, "DIFFERENTIAL");

  r = nmeaInfoSignalToString(NMEALIB_SIG_SENSITIVE);
  CU_ASSERT_STRING_EQUAL(r, "SENSITIVE");

  r = nmeaInfoSignalToString(NMEALIB_SIG_RTKIN);
  CU_ASSERT_STRING_EQUAL(r, "REAL TIME KINEMATIC");

  r = nmeaInfoSignalToString(NMEALIB_SIG_FLOAT_RTK);
  CU_ASSERT_STRING_EQUAL(r, "FLOAT REAL TIME KINEMATIC");

  r = nmeaInfoSignalToString(NMEALIB_SIG_ESTIMATED);
  CU_ASSERT_STRING_EQUAL(r, "ESTIMATED (DEAD RECKONING)");

  r = nmeaInfoSignalToString(NMEALIB_SIG_MANUAL);
  CU_ASSERT_STRING_EQUAL(r, "MANUAL");

  r = nmeaInfoSignalToString(NMEALIB_SIG_SIMULATION);
  CU_ASSERT_STRING_EQUAL(r, "SIMULATION");

  r = nmeaInfoSignalToString(NMEALIB_SIG_LAST + 1);
  CU_ASSERT_PTR_NULL(r);
}

static void test_nmeaInfoModeToSig(void) {
  int r;

  r = nmeaInfoModeToSignal('N');
  CU_ASSERT_EQUAL(r, NMEALIB_SIG_INVALID);

  r = nmeaInfoModeToSignal('A');
  CU_ASSERT_EQUAL(r, NMEALIB_SIG_FIX);

  r = nmeaInfoModeToSignal('D');
  CU_ASSERT_EQUAL(r, NMEALIB_SIG_DIFFERENTIAL);

  r = nmeaInfoModeToSignal('P');
  CU_ASSERT_EQUAL(r, NMEALIB_SIG_SENSITIVE);

  r = nmeaInfoModeToSignal('R');
  CU_ASSERT_EQUAL(r, NMEALIB_SIG_RTKIN);

  r = nmeaInfoModeToSignal('F');
  CU_ASSERT_EQUAL(r, NMEALIB_SIG_FLOAT_RTK);

  r = nmeaInfoModeToSignal('E');
  CU_ASSERT_EQUAL(r, NMEALIB_SIG_ESTIMATED);

  r = nmeaInfoModeToSignal('M');
  CU_ASSERT_EQUAL(r, NMEALIB_SIG_MANUAL);

  r = nmeaInfoModeToSignal('S');
  CU_ASSERT_EQUAL(r, NMEALIB_SIG_SIMULATION);

  r = nmeaInfoModeToSignal('\1');
  CU_ASSERT_EQUAL(r, NMEALIB_SIG_INVALID);
}

static void test_nmeaInfoSigToMode(void) {
  char r;

  r = nmeaInfoSignalToMode(NMEALIB_SIG_INVALID);
  CU_ASSERT_EQUAL(r, 'N');

  r = nmeaInfoSignalToMode(NMEALIB_SIG_FIX);
  CU_ASSERT_EQUAL(r, 'A');

  r = nmeaInfoSignalToMode(NMEALIB_SIG_DIFFERENTIAL);
  CU_ASSERT_EQUAL(r, 'D');

  r = nmeaInfoSignalToMode(NMEALIB_SIG_SENSITIVE);
  CU_ASSERT_EQUAL(r, 'P');

  r = nmeaInfoSignalToMode(NMEALIB_SIG_RTKIN);
  CU_ASSERT_EQUAL(r, 'R');

  r = nmeaInfoSignalToMode(NMEALIB_SIG_FLOAT_RTK);
  CU_ASSERT_EQUAL(r, 'F');

  r = nmeaInfoSignalToMode(NMEALIB_SIG_ESTIMATED);
  CU_ASSERT_EQUAL(r, 'E');

  r = nmeaInfoSignalToMode(NMEALIB_SIG_MANUAL);
  CU_ASSERT_EQUAL(r, 'M');

  r = nmeaInfoSignalToMode(NMEALIB_SIG_SIMULATION);
  CU_ASSERT_EQUAL(r, 'S');

  r = nmeaInfoSignalToMode(NMEALIB_SIG_LAST + 1);
  CU_ASSERT_EQUAL(r, 'N');
}

static void test_nmeaInfoFixToString(void) {
  const char * r;

  r = nmeaInfoFixToString(NMEALIB_FIX_FIRST - 1);
  CU_ASSERT_PTR_NULL(r);

  r = nmeaInfoFixToString(NMEALIB_FIX_BAD);
  CU_ASSERT_STRING_EQUAL(r, "BAD");

  r = nmeaInfoFixToString(NMEALIB_FIX_2D);
  CU_ASSERT_STRING_EQUAL(r, "2D");

  r = nmeaInfoFixToString(NMEALIB_FIX_3D);
  CU_ASSERT_STRING_EQUAL(r, "3D");

  r = nmeaInfoFixToString(NMEALIB_FIX_LAST + 1);
  CU_ASSERT_PTR_NULL(r);
}

static void test_nmeaInfoFieldToString(void) {
  const char * r;

  r = nmeaInfoFieldToString(0);
  CU_ASSERT_PTR_NULL(r);

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_SMASK);
  CU_ASSERT_STRING_EQUAL(r, "SMASK");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_UTCDATE);
  CU_ASSERT_STRING_EQUAL(r, "UTCDATE");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_UTCTIME);
  CU_ASSERT_STRING_EQUAL(r, "UTCTIME");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_SIG);
  CU_ASSERT_STRING_EQUAL(r, "SIG");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_FIX);
  CU_ASSERT_STRING_EQUAL(r, "FIX");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_PDOP);
  CU_ASSERT_STRING_EQUAL(r, "PDOP");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_HDOP);
  CU_ASSERT_STRING_EQUAL(r, "HDOP");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_VDOP);
  CU_ASSERT_STRING_EQUAL(r, "VDOP");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_LAT);
  CU_ASSERT_STRING_EQUAL(r, "LAT");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_LON);
  CU_ASSERT_STRING_EQUAL(r, "LON");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_ELV);
  CU_ASSERT_STRING_EQUAL(r, "ELV");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_SPEED);
  CU_ASSERT_STRING_EQUAL(r, "SPEED");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_TRACK);
  CU_ASSERT_STRING_EQUAL(r, "TRACK");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_MTRACK);
  CU_ASSERT_STRING_EQUAL(r, "MTRACK");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_MAGVAR);
  CU_ASSERT_STRING_EQUAL(r, "MAGVAR");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_SATINUSECOUNT);
  CU_ASSERT_STRING_EQUAL(r, "SATINUSECOUNT");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_SATINUSE);
  CU_ASSERT_STRING_EQUAL(r, "SATINUSE");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_SATINVIEWCOUNT);
  CU_ASSERT_STRING_EQUAL(r, "SATINVIEWCOUNT");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_STRING_EQUAL(r, "SATINVIEW");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_HEIGHT);
  CU_ASSERT_STRING_EQUAL(r, "HEIGHT");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_DGPSAGE);
  CU_ASSERT_STRING_EQUAL(r, "DGPSAGE");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_DGPSSID);
  CU_ASSERT_STRING_EQUAL(r, "DGPSSID");

  r = nmeaInfoFieldToString(NMEALIB_PRESENT_LAST + 1);
  CU_ASSERT_PTR_NULL(r);
}

static void test_nmeaInfoIsPresentAll(void) {
  bool r;

  r = nmeaInfoIsPresentAll(0xa, 0xa);
  CU_ASSERT_EQUAL(r, true);

  r = nmeaInfoIsPresentAll(0xa, 0x3);
  CU_ASSERT_EQUAL(r, false);

  r = nmeaInfoIsPresentAll(0xa, 0x2);
  CU_ASSERT_EQUAL(r, true);

  r = nmeaInfoIsPresentAll(0xa, 0x5);
  CU_ASSERT_EQUAL(r, false);
}

static void test_nmeaInfoIsPresentAny(void) {
  bool r;

  r = nmeaInfoIsPresentAny(0xa, 0xa);
  CU_ASSERT_EQUAL(r, true);

  r = nmeaInfoIsPresentAny(0xa, 0x3);
  CU_ASSERT_EQUAL(r, true);

  r = nmeaInfoIsPresentAny(0xa, 0x2);
  CU_ASSERT_EQUAL(r, true);

  r = nmeaInfoIsPresentAny(0xa, 0x5);
  CU_ASSERT_EQUAL(r, false);
}

static void test_nmeaInfoSetPresent(void) {
  uint32_t r;

  r = 0x5;
  nmeaInfoSetPresent(&r, 0xa);
  CU_ASSERT_EQUAL(r, 0xf);

  r = 0x5;
  nmeaInfoSetPresent(&r, 0x3);
  CU_ASSERT_EQUAL(r, 0x7);

  r = 0x5;
  nmeaInfoSetPresent(&r, 0x2);
  CU_ASSERT_EQUAL(r, 0x7);

  r = 0x5;
  nmeaInfoSetPresent(&r, 0x5);
  CU_ASSERT_EQUAL(r, 0x5);
}

static void test_nmeaInfoUnsetPresent(void) {
  uint32_t r;

  r = 0xa;
  nmeaInfoUnsetPresent(&r, 0xa);
  CU_ASSERT_EQUAL(r, 0x0);

  r = 0xa;
  nmeaInfoUnsetPresent(&r, 0x3);
  CU_ASSERT_EQUAL(r, 0x8);

  r = 0xa;
  nmeaInfoUnsetPresent(&r, 0x2);
  CU_ASSERT_EQUAL(r, 0x8);

  r = 0xa;
  nmeaInfoUnsetPresent(&r, 0x5);
  CU_ASSERT_EQUAL(r, 0xa);
}

static void test_nmeaTimeParseTime(void) {
  bool r;
  const char *time;
  NmeaTime t;

  /* invalid inputs */

  r = nmeaTimeParseTime(NULL, &t);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);

  time = "invalid";
  r = nmeaTimeParseTime(time, NULL);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);

  /* internal whitespace */

  memset(&t, 0xff, sizeof(t));
  time = "12 456";
  r = nmeaTimeParseTime(time, &t);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);

  /* length 5 */

  memset(&t, 0xff, sizeof(t));
  time = " 12345";
  r = nmeaTimeParseTime(time, &t);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);

  /* length 6 with external whitespace */

  memset(&t, 0xff, sizeof(t));
  time = " 123456 ";
  r = nmeaTimeParseTime(time, &t);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(t.hour, 12);
  CU_ASSERT_EQUAL(t.min, 34);
  CU_ASSERT_EQUAL(t.sec, 56);
  CU_ASSERT_EQUAL(t.hsec, 00);

  memset(&t, 0xff, sizeof(t));
  time = " 12qq56 ";
  r = nmeaTimeParseTime(time, &t);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  /* length 7 */

  memset(&t, 0xff, sizeof(t));
  time = " 123456.";
  r = nmeaTimeParseTime(time, &t);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);

  /* length 8 */

  memset(&t, 0xff, sizeof(t));
  time = "123456.7";
  r = nmeaTimeParseTime(time, &t);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(t.hour, 12);
  CU_ASSERT_EQUAL(t.min, 34);
  CU_ASSERT_EQUAL(t.sec, 56);
  CU_ASSERT_EQUAL(t.hsec, 70);

  memset(&t, 0xff, sizeof(t));
  time = "12q456.7";
  r = nmeaTimeParseTime(time, &t);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  /* length 9 */

  memset(&t, 0xff, sizeof(t));
  time = "123456.78";
  r = nmeaTimeParseTime(time, &t);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(t.hour, 12);
  CU_ASSERT_EQUAL(t.min, 34);
  CU_ASSERT_EQUAL(t.sec, 56);
  CU_ASSERT_EQUAL(t.hsec, 78);

  memset(&t, 0xff, sizeof(t));
  time = "123456.q8";
  r = nmeaTimeParseTime(time, &t);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  /* length 10 */

  memset(&t, 0xff, sizeof(t));
  time = "123456.789";
  r = nmeaTimeParseTime(time, &t);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(t.hour, 12);
  CU_ASSERT_EQUAL(t.min, 34);
  CU_ASSERT_EQUAL(t.sec, 56);
  CU_ASSERT_EQUAL(t.hsec, 79);

  memset(&t, 0xff, sizeof(t));
  time = "123456.q89";
  r = nmeaTimeParseTime(time, &t);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);

  /* length 11 */

  memset(&t, 0xff, sizeof(t));
  time = "123456.7891";
  r = nmeaTimeParseTime(time, &t);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);
}

static void test_nmeaTimeParseDate(void) {
  bool r;
  const char *date;
  NmeaTime d;

  /* invalid inputs */

  r = nmeaTimeParseDate(NULL, &d);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);

  date = "invalid";
  r = nmeaTimeParseDate(date, NULL);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);

  /* internal whitespace */

  memset(&d, 0xff, sizeof(d));
  date = "12 456";
  r = nmeaTimeParseDate(date, &d);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);

  /* length 5 */

  memset(&d, 0xff, sizeof(d));
  date = " 12345";
  r = nmeaTimeParseDate(date, &d);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 0);

  /* length 6 with external whitespace */

  memset(&d, 0xff, sizeof(d));
  date = " 123456 ";
  r = nmeaTimeParseDate(date, &d);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(d.day, 12);
  CU_ASSERT_EQUAL(d.mon, 34);
  CU_ASSERT_EQUAL(d.year, 2056);

  memset(&d, 0xff, sizeof(d));
  date = " 123492 ";
  r = nmeaTimeParseDate(date, &d);
  CU_ASSERT_EQUAL(r, true);
  validateContext(0, 0);
  CU_ASSERT_EQUAL(d.day, 12);
  CU_ASSERT_EQUAL(d.mon, 34);
  CU_ASSERT_EQUAL(d.year, 1992);

  memset(&d, 0xff, sizeof(d));
  date = " 12qq56 ";
  r = nmeaTimeParseDate(date, &d);
  CU_ASSERT_EQUAL(r, false);
  validateContext(0, 1);
}

static void test_nmeaTimeSet(void) {
  NmeaTime utcClean;
  uint32_t present;
  struct timeval timeval;
  struct tm tt;
  NmeaTime utc;
  NmeaTime utcExpected;

  utcClean.day = 23;
  utcClean.mon = 12;
  utcClean.year = 2016;
  utcClean.hour = 21;
  utcClean.min = 42;
  utcClean.sec = 51;
  utcClean.hsec = 88;
  present = 0;
  memset(&timeval, 0, sizeof(timeval));

  /* invalid inputs */

  present = 0;
  memcpy(&utc, &utcClean, sizeof(utcClean));
  memcpy(&utcExpected, &utcClean, sizeof(utcClean));
  nmeaTimeSet(NULL, &present, &timeval);
  CU_ASSERT_EQUAL(memcmp(&utc, &utcExpected, sizeof(utc)), 0);
  CU_ASSERT_EQUAL(present, 0);

  /* with timeval */

  present = 0;
  memcpy(&utc, &utcClean, sizeof(utcClean));
  memcpy(&utcExpected, &utcClean, sizeof(utcClean));
  timeval.tv_sec = 0;
  timeval.tv_usec = 420000;
  utcExpected.year = 1970;
  utcExpected.mon = 1;
  utcExpected.day = 1;
  utcExpected.hour = 0;
  utcExpected.min = 0;
  utcExpected.sec = 0;
  utcExpected.hsec = 42;
  nmeaTimeSet(&utc, NULL, &timeval);
  CU_ASSERT_EQUAL(memcmp(&utc, &utcExpected, sizeof(utc)), 0);
  CU_ASSERT_EQUAL(present, 0);

  present = 0;
  memcpy(&utc, &utcClean, sizeof(utcClean));
  memcpy(&utcExpected, &utcClean, sizeof(utcClean));
  timeval.tv_sec = 0;
  timeval.tv_usec = 420000;
  utcExpected.year = 1970;
  utcExpected.mon = 1;
  utcExpected.day = 1;
  utcExpected.hour = 0;
  utcExpected.min = 0;
  utcExpected.sec = 0;
  utcExpected.hsec = 42;
  nmeaTimeSet(&utc, &present, &timeval);
  CU_ASSERT_EQUAL(memcmp(&utc, &utcExpected, sizeof(utc)), 0);
  CU_ASSERT_EQUAL(present, NMEALIB_PRESENT_UTCDATE | NMEALIB_PRESENT_UTCTIME);

  /* without timeval */

  present = 0;
  memcpy(&utc, &utcClean, sizeof(utcClean));
  memcpy(&utcExpected, &utcClean, sizeof(utcClean));
  gettimeofday(&timeval, NULL);
  nmeaTimeSet(&utc, NULL, NULL);
  gmtime_r(&timeval.tv_sec, &tt);
  utcExpected.year = (unsigned int) tt.tm_year + 1900;
  utcExpected.mon = (unsigned int) tt.tm_mon + 1;
  utcExpected.day = (unsigned int) tt.tm_mday;
  utcExpected.hour = (unsigned int) tt.tm_hour;
  utcExpected.min = (unsigned int) tt.tm_min;
  utcExpected.sec = (unsigned int) tt.tm_sec;
  utcExpected.hsec = utc.hsec;
  CU_ASSERT_EQUAL(memcmp(&utc, &utcExpected, sizeof(utc)), 0);
  CU_ASSERT_EQUAL(present, 0);

  present = 0;
  memcpy(&utc, &utcClean, sizeof(utcClean));
  memcpy(&utcExpected, &utcClean, sizeof(utcClean));
  gettimeofday(&timeval, NULL);
  nmeaTimeSet(&utc, &present, NULL);
  gmtime_r(&timeval.tv_sec, &tt);
  utcExpected.year = (unsigned int) tt.tm_year + 1900;
  utcExpected.mon = (unsigned int) tt.tm_mon + 1;
  utcExpected.day = (unsigned int) tt.tm_mday;
  utcExpected.hour = (unsigned int) tt.tm_hour;
  utcExpected.min = (unsigned int) tt.tm_min;
  utcExpected.sec = (unsigned int) tt.tm_sec;
  utcExpected.hsec = utc.hsec;
  CU_ASSERT_EQUAL(memcmp(&utc, &utcExpected, sizeof(utc)), 0);
  CU_ASSERT_EQUAL(present, NMEALIB_PRESENT_UTCDATE | NMEALIB_PRESENT_UTCTIME);
}

static void test_nmeaInfoClear(void) {
  NmeaInfo info;
  NmeaInfo infoExpected;

  /* invalid inputs */

  memset(&info, 0xaa, sizeof(info));
  memcpy(&infoExpected, &info, sizeof(info));
  nmeaInfoClear(NULL);
  CU_ASSERT_EQUAL(memcmp(&info, &infoExpected, sizeof(info)), 0);

  /* normal */

  memset(&info, 0xaa, sizeof(info));
  memset(&infoExpected, 0, sizeof(infoExpected));
  infoExpected.sig = NMEALIB_SIG_INVALID;
  infoExpected.fix = NMEALIB_FIX_BAD;
  infoExpected.present = NMEALIB_PRESENT_SIG | NMEALIB_PRESENT_FIX;
  nmeaInfoClear(&info);
  CU_ASSERT_EQUAL(memcmp(&info, &infoExpected, sizeof(info)), 0);
}

static void test_nmeaInfoSanitise(void) {
  NmeaInfo infoEmpty;
  NmeaInfo infoEmptyFF;
  NmeaInfo info;
  NmeaTime utc;

  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&infoEmptyFF, 0xff, sizeof(infoEmptyFF));

  /* invalid input */

  memset(&info, 0, sizeof(info));
  nmeaInfoSanitise(NULL);

  /* nothing present */

  memset(&info, 0xff, sizeof(info));
  info.present = 0;
  info.progress.gpgsvInProgress = true;
  nmeaTimeSet(&utc, NULL, NULL);
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.present, 0);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, utc.year);
  CU_ASSERT_EQUAL(info.utc.mon, utc.mon);
  CU_ASSERT_EQUAL(info.utc.day, utc.day);
  CU_ASSERT_EQUAL(info.utc.hour, utc.hour);
  CU_ASSERT_EQUAL(info.utc.min, utc.min);
  CU_ASSERT_EQUAL(info.utc.sec, utc.sec);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_INVALID);
  CU_ASSERT_EQUAL(info.fix, NMEALIB_FIX_BAD);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, NMEALIB_LATITUDE_DEFAULT_NDEG, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, NMEALIB_LONGITUDE_DEFAULT_NDEG, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.dgpsSid, 0);
  CU_ASSERT_EQUAL(info.satellites.inUseCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inUse, infoEmpty.satellites.inUse, sizeof(info.satellites.inUse)), 0);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inView, infoEmptyFF.satellites.inView, sizeof(info.satellites.inView)), 0);
  CU_ASSERT_EQUAL(info.progress.gpgsvInProgress, true);
  CU_ASSERT_EQUAL(info.metric, false);

  memset(&info, 0xff, sizeof(info));
  info.present = 0;
  info.progress.gpgsvInProgress = false;
  nmeaTimeSet(&utc, NULL, NULL);
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.present, 0);
  CU_ASSERT_EQUAL(info.smask, 0);
  CU_ASSERT_EQUAL(info.utc.year, utc.year);
  CU_ASSERT_EQUAL(info.utc.mon, utc.mon);
  CU_ASSERT_EQUAL(info.utc.day, utc.day);
  CU_ASSERT_EQUAL(info.utc.hour, utc.hour);
  CU_ASSERT_EQUAL(info.utc.min, utc.min);
  CU_ASSERT_EQUAL(info.utc.sec, utc.sec);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_INVALID);
  CU_ASSERT_EQUAL(info.fix, NMEALIB_FIX_BAD);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, NMEALIB_LATITUDE_DEFAULT_NDEG, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, NMEALIB_LONGITUDE_DEFAULT_NDEG, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 0.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(info.dgpsSid, 0);
  CU_ASSERT_EQUAL(info.satellites.inUseCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inUse, infoEmpty.satellites.inUse, sizeof(info.satellites.inUse)), 0);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  CU_ASSERT_EQUAL(memcmp(info.satellites.inView, infoEmpty.satellites.inView, sizeof(info.satellites.inView)), 0);
  CU_ASSERT_EQUAL(info.progress.gpgsvInProgress, false);
  CU_ASSERT_EQUAL(info.metric, false);

  /* present & smask */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SMASK | (UINT32_MAX & ~(UINT32_MAX >> 1));
  info.smask = UINT32_MAX;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_MASK);

  /* utc */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_UTCDATE | NMEALIB_PRESENT_UTCTIME;
  memset(&info.utc, 0, sizeof(info.utc));
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.utc.year, 1990);
  CU_ASSERT_EQUAL(info.utc.mon, 1);
  CU_ASSERT_EQUAL(info.utc.day, 1);
  CU_ASSERT_EQUAL(info.utc.hour, 0);
  CU_ASSERT_EQUAL(info.utc.min, 0);
  CU_ASSERT_EQUAL(info.utc.sec, 0);
  CU_ASSERT_EQUAL(info.utc.hsec, 0);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_UTCDATE | NMEALIB_PRESENT_UTCTIME;
  memset(&info.utc, 0xff, sizeof(info.utc));
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.utc.year, 2189);
  CU_ASSERT_EQUAL(info.utc.mon, 12);
  CU_ASSERT_EQUAL(info.utc.day, 31);
  CU_ASSERT_EQUAL(info.utc.hour, 15);
  CU_ASSERT_EQUAL(info.utc.min, 15);
  CU_ASSERT_EQUAL(info.utc.sec, 56);
  CU_ASSERT_EQUAL(info.utc.hsec, 95);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_UTCDATE | NMEALIB_PRESENT_UTCTIME;
  info.utc.year = 2016;
  info.utc.mon = 7;
  info.utc.day = 8;
  info.utc.hour = 18;
  info.utc.min = 19;
  info.utc.sec = 20;
  info.utc.hsec = 21;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.utc.year, 2016);
  CU_ASSERT_EQUAL(info.utc.mon, 7);
  CU_ASSERT_EQUAL(info.utc.day, 8);
  CU_ASSERT_EQUAL(info.utc.hour, 18);
  CU_ASSERT_EQUAL(info.utc.min, 19);
  CU_ASSERT_EQUAL(info.utc.sec, 20);
  CU_ASSERT_EQUAL(info.utc.hsec, 21);

  /* sig */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SIG;
  info.sig = NMEALIB_SIG_FIRST - 1;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_INVALID);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SIG;
  info.sig = NMEALIB_SIG_LAST + 1;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_INVALID);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SIG;
  info.sig = NMEALIB_SIG_FLOAT_RTK;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_FLOAT_RTK);

  /* fix */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_FIX;
  info.fix = NMEALIB_FIX_FIRST - 1;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.fix, NMEALIB_FIX_BAD);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_FIX;
  info.fix = NMEALIB_FIX_LAST + 1;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.fix, NMEALIB_FIX_BAD);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_FIX;
  info.fix = NMEALIB_FIX_3D;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.fix, NMEALIB_FIX_3D);

  /* pdop */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_PDOP;
  info.pdop = -42.45;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 42.45, FLT_EPSILON);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_PDOP;
  info.pdop = 42.45;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 42.45, FLT_EPSILON);

  /* hdop */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_HDOP;
  info.hdop = -42.45;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 42.45, FLT_EPSILON);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_HDOP;
  info.hdop = 42.45;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 42.45, FLT_EPSILON);

  /* vdop */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_VDOP;
  info.vdop = -42.45;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 42.45, FLT_EPSILON);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_VDOP;
  info.vdop = 42.45;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 42.45, FLT_EPSILON);

  /* lat & lon normal */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_LAT | NMEALIB_PRESENT_LON;
  info.latitude = -4200.45;
  info.longitude = -4200.45;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, -4200.45, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, -4200.45, FLT_EPSILON);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_LAT | NMEALIB_PRESENT_LON;
  info.latitude = 4200.45;
  info.longitude = 4200.45;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, 4200.45, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 4200.45, FLT_EPSILON);

  /* lat out of range */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_LAT | NMEALIB_PRESENT_LON;
  info.latitude = -22200.5;
  info.longitude = -4200.5;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, 4200.5, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 13799.5, FLT_EPSILON);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_LAT | NMEALIB_PRESENT_LON;
  info.latitude = 22200.5;
  info.longitude = -4200.5;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, -4200.5, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 13799.5, FLT_EPSILON);

  /* lon out of range */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_LAT | NMEALIB_PRESENT_LON;
  info.latitude = -4200.5;
  info.longitude = -22200.5;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, -4200.5, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 13799.5, FLT_EPSILON);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_LAT | NMEALIB_PRESENT_LON;
  info.latitude = -4200.5;
  info.longitude = 22200.5;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, -4200.5, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, -13799.5, FLT_EPSILON);

  /* elv: no range adjustments */

  /* height: no range adjustments */

  /* speed & track & mtrack normal */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SPEED | NMEALIB_PRESENT_TRACK | NMEALIB_PRESENT_MTRACK;
  info.speed = 10.0;
  info.track = 45.0;
  info.mtrack = 55.0;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 10.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 45.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 55.0, FLT_EPSILON);

  /* speed */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SPEED | NMEALIB_PRESENT_TRACK | NMEALIB_PRESENT_MTRACK;
  info.speed = -10.0;
  info.track = 45.0;
  info.mtrack = 55.0;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 10.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 225.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 235.0, FLT_EPSILON);

  /* track */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SPEED | NMEALIB_PRESENT_TRACK | NMEALIB_PRESENT_MTRACK;
  info.speed = 10.0;
  info.track = -45.0;
  info.mtrack = 55.0;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 10.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 315.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 55.0, FLT_EPSILON);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SPEED | NMEALIB_PRESENT_TRACK | NMEALIB_PRESENT_MTRACK;
  info.speed = 10.0;
  info.track = 405.0;
  info.mtrack = 55.0;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 10.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 45.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 55.0, FLT_EPSILON);

  /* mtrack */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SPEED | NMEALIB_PRESENT_TRACK | NMEALIB_PRESENT_MTRACK;
  info.speed = 10.0;
  info.track = 45.0;
  info.mtrack = -55.0;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 10.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 45.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 305.0, FLT_EPSILON);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SPEED | NMEALIB_PRESENT_TRACK | NMEALIB_PRESENT_MTRACK;
  info.speed = 10.0;
  info.track = 45.0;
  info.mtrack = 415.0;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 10.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 45.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 55.0, FLT_EPSILON);

  /* magvar */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_MAGVAR;
  info.magvar = 55.0;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 55.0, FLT_EPSILON);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_MAGVAR;
  info.magvar = -55.0;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 305.0, FLT_EPSILON);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_MAGVAR;
  info.magvar = 415.0;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 55.0, FLT_EPSILON);

  /* dgpsAge */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_DGPSAGE;
  info.dgpsAge = -42.45;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 42.45, FLT_EPSILON);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_DGPSAGE;
  info.dgpsAge = 42.45;
  nmeaInfoSanitise(&info);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 42.45, FLT_EPSILON);

  /* dgpsSid */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_DGPSSID;
  info.dgpsSid = 42;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.dgpsSid, 42);

  /* sat in use count */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SATINUSECOUNT;
  info.satellites.inUseCount = 42;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.satellites.inUseCount, 42);

  /* sat in use */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SATINUSE;
  info.satellites.inUse[0] = 0;
  info.satellites.inUse[1] = 10;
  info.satellites.inUse[2] = 5;
  info.satellites.inUse[3] = 4;
  info.satellites.inUse[4] = 3;
  info.satellites.inUse[5] = 22;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.satellites.inUse[0], 10);
  CU_ASSERT_EQUAL(info.satellites.inUse[1], 5);
  CU_ASSERT_EQUAL(info.satellites.inUse[2], 4);
  CU_ASSERT_EQUAL(info.satellites.inUse[3], 3);
  CU_ASSERT_EQUAL(info.satellites.inUse[4], 22);
  CU_ASSERT_EQUAL(info.satellites.inUse[5], 0);

  /* sat in view count */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SATINVIEWCOUNT;
  info.satellites.inViewCount = 42;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 42);

  /* sat in view */

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SATINVIEW;
  info.satellites.inView[0].prn = 0;
  info.satellites.inView[1].prn = 10;
  info.satellites.inView[2].prn = 5;
  info.satellites.inView[3].prn = 4;
  info.satellites.inView[4].prn = 3;
  info.satellites.inView[5].prn = 22;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.satellites.inView[0].prn, 10);
  CU_ASSERT_EQUAL(info.satellites.inView[1].prn, 5);
  CU_ASSERT_EQUAL(info.satellites.inView[2].prn, 4);
  CU_ASSERT_EQUAL(info.satellites.inView[3].prn, 3);
  CU_ASSERT_EQUAL(info.satellites.inView[4].prn, 22);
  CU_ASSERT_EQUAL(info.satellites.inView[5].prn, 0);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SATINVIEW;
  info.satellites.inView[0].prn = 10;
  info.satellites.inView[0].elevation = 20;
  info.satellites.inView[0].azimuth = 30;
  info.satellites.inView[0].snr = 40;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.satellites.inView[0].prn, 10);
  CU_ASSERT_EQUAL(info.satellites.inView[0].elevation, 20);
  CU_ASSERT_EQUAL(info.satellites.inView[0].azimuth, 30);
  CU_ASSERT_EQUAL(info.satellites.inView[0].snr, 40);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SATINVIEW;
  info.satellites.inView[0].prn = 10;
  info.satellites.inView[0].elevation = -200;
  info.satellites.inView[0].azimuth = 30;
  info.satellites.inView[0].snr = 40;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.satellites.inView[0].prn, 10);
  CU_ASSERT_EQUAL(info.satellites.inView[0].elevation, 20);
  CU_ASSERT_EQUAL(info.satellites.inView[0].azimuth, 210);
  CU_ASSERT_EQUAL(info.satellites.inView[0].snr, 40);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SATINVIEW;
  info.satellites.inView[0].prn = 10;
  info.satellites.inView[0].elevation = 200;
  info.satellites.inView[0].azimuth = 30;
  info.satellites.inView[0].snr = 40;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.satellites.inView[0].prn, 10);
  CU_ASSERT_EQUAL(info.satellites.inView[0].elevation, -20);
  CU_ASSERT_EQUAL(info.satellites.inView[0].azimuth, 210);
  CU_ASSERT_EQUAL(info.satellites.inView[0].snr, 40);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SATINVIEW;
  info.satellites.inView[0].prn = 10;
  info.satellites.inView[0].elevation = 20;
  info.satellites.inView[0].azimuth = 390;
  info.satellites.inView[0].snr = 40;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.satellites.inView[0].prn, 10);
  CU_ASSERT_EQUAL(info.satellites.inView[0].elevation, 20);
  CU_ASSERT_EQUAL(info.satellites.inView[0].azimuth, 30);
  CU_ASSERT_EQUAL(info.satellites.inView[0].snr, 40);

  memset(&info, 0, sizeof(info));
  info.present = NMEALIB_PRESENT_SATINVIEW;
  info.satellites.inView[0].prn = 10;
  info.satellites.inView[0].elevation = 20;
  info.satellites.inView[0].azimuth = 30;
  info.satellites.inView[0].snr = 140;
  nmeaInfoSanitise(&info);
  CU_ASSERT_EQUAL(info.satellites.inView[0].prn, 10);
  CU_ASSERT_EQUAL(info.satellites.inView[0].elevation, 20);
  CU_ASSERT_EQUAL(info.satellites.inView[0].azimuth, 30);
  CU_ASSERT_EQUAL(info.satellites.inView[0].snr, 99);
}

static void test_nmeaInfoUnitConversion(void) {
  NmeaInfo info;
  NmeaInfo infoClean;
  NmeaInfo infoExpected;

  memset(&infoClean, 0xaa, sizeof(infoClean));
  infoClean.present = NMEALIB_INFO_PRESENT_MASK;
  infoClean.smask = NMEALIB_SENTENCE_MASK;
  infoClean.utc.day = 23;
  infoClean.utc.mon = 12;
  infoClean.utc.year = 2016;
  infoClean.utc.hour = 21;
  infoClean.utc.min = 42;
  infoClean.utc.sec = 51;
  infoClean.utc.hsec = 88;
  infoClean.sig = NMEALIB_SIG_FLOAT_RTK;
  infoClean.fix = NMEALIB_FIX_3D;
  infoClean.pdop = 1.1;
  infoClean.hdop = 2.2;
  infoClean.vdop = 3.3;
  infoClean.latitude = 4.4;
  infoClean.longitude = 5.5;
  infoClean.elevation = 6.6;
  infoClean.height = 7.7;
  infoClean.speed = 8.8;
  infoClean.track = 9.9;
  infoClean.mtrack = 11.11;
  infoClean.magvar = 12.12;
  infoClean.dgpsAge = 13.13;
  infoClean.dgpsSid = 114;
  memset(&infoClean.satellites, 0xaa, sizeof(infoClean.satellites));
  infoClean.progress.gpgsvInProgress = true;

  /* invalid inputs */

  memcpy(&info, &infoClean, sizeof(infoClean));
  info.metric = false;
  memcpy(&infoExpected, &info, sizeof(info));
  nmeaInfoUnitConversion(NULL, true);
  CU_ASSERT_EQUAL(memcmp(&info, &infoExpected, sizeof(info)), 0);

  /* already metric */

  memcpy(&info, &infoClean, sizeof(infoClean));
  info.metric = true;
  memcpy(&infoExpected, &info, sizeof(info));
  nmeaInfoUnitConversion(&info, true);
  CU_ASSERT_EQUAL(memcmp(&info, &infoExpected, sizeof(info)), 0);

  /* already non-metric */

  memcpy(&info, &infoClean, sizeof(infoClean));
  info.metric = false;
  memcpy(&infoExpected, &info, sizeof(info));
  nmeaInfoUnitConversion(&info, false);
  CU_ASSERT_EQUAL(memcmp(&info, &infoExpected, sizeof(info)), 0);

  /* to metric */

  memcpy(&info, &infoClean, sizeof(infoClean));
  info.metric = false;
  memcpy(&infoExpected, &info, sizeof(info));
  infoExpected.metric = true;
  infoExpected.pdop = nmeaMathDopToMeters(infoExpected.pdop);
  infoExpected.hdop = nmeaMathDopToMeters(infoExpected.hdop);
  infoExpected.vdop = nmeaMathDopToMeters(infoExpected.vdop);
  infoExpected.latitude = nmeaMathNdegToDegree(infoExpected.latitude);
  infoExpected.longitude = nmeaMathNdegToDegree(infoExpected.longitude);
  nmeaInfoUnitConversion(&info, true);
  CU_ASSERT_EQUAL(memcmp(&info, &infoExpected, sizeof(info)), 0);

  /* to non-metric */

  memcpy(&info, &infoClean, sizeof(infoClean));
  info.metric = true;
  memcpy(&infoExpected, &info, sizeof(info));
  infoExpected.metric = false;
  infoExpected.pdop = nmeaMathMetersToDop(infoExpected.pdop);
  infoExpected.hdop = nmeaMathMetersToDop(infoExpected.hdop);
  infoExpected.vdop = nmeaMathMetersToDop(infoExpected.vdop);
  infoExpected.latitude = nmeaMathDegreeToNdeg(infoExpected.latitude);
  infoExpected.longitude = nmeaMathDegreeToNdeg(infoExpected.longitude);
  nmeaInfoUnitConversion(&info, false);
  CU_ASSERT_EQUAL(memcmp(&info, &infoExpected, sizeof(info)), 0);

  /* to metric, but nothing present */

  memcpy(&info, &infoClean, sizeof(infoClean));
  info.present = 0;
  info.metric = false;
  memcpy(&infoExpected, &info, sizeof(info));
  infoExpected.present = 0;
  infoExpected.metric = true;
  nmeaInfoUnitConversion(&info, true);
  CU_ASSERT_EQUAL(memcmp(&info, &infoExpected, sizeof(info)), 0);
}

static void test_nmeaQsortPRNCompare(void) {
  unsigned int prn1;
  unsigned int prn2;
  int r;

  unsigned int prn[] = {
      2,
      3,
      4,
      0,
      1,
      11,
      0,
      14,
      10,
      9,
      8,
      0,
      7,
      0 };
  unsigned int exp[] = {
      1,
      2,
      3,
      4,
      7,
      8,
      9,
      10,
      11,
      14,
      0,
      0,
      0,
      0 };

  qsort(&prn, sizeof(prn) / sizeof(prn[0]), sizeof(prn[0]), nmeaQsortPRNCompare);
  CU_ASSERT_EQUAL(memcmp(prn, exp, sizeof(prn)), 0);

  prn1 = 0;
  prn2 = 0;
  r = nmeaQsortPRNCompare(&prn1, &prn2);
  CU_ASSERT_EQUAL(r, 0);

  prn1 = 1;
  prn2 = 0;
  r = nmeaQsortPRNCompare(&prn1, &prn2);
  CU_ASSERT_EQUAL(r, -1);

  prn1 = 0;
  prn2 = 1;
  r = nmeaQsortPRNCompare(&prn1, &prn2);
  CU_ASSERT_EQUAL(r, 1);

  prn1 = 1;
  prn2 = 1;
  r = nmeaQsortPRNCompare(&prn1, &prn2);
  CU_ASSERT_EQUAL(r, 0);

  prn1 = 1;
  prn2 = 2;
  r = nmeaQsortPRNCompare(&prn1, &prn2);
  CU_ASSERT_EQUAL(r, -1);

  prn1 = 2;
  prn2 = 1;
  r = nmeaQsortPRNCompare(&prn1, &prn2);
  CU_ASSERT_EQUAL(r, 1);
}

static void test_nmeaQsortPRNCompact(void) {
  unsigned int prn1;
  unsigned int prn2;
  int r;

  unsigned int prn[] = {
      2,
      4,
      3,
      0,
      1,
      11,
      0,
      14,
      10,
      9,
      8,
      0,
      7,
      0 };
  unsigned int exp[] = {
      2,
      4,
      3,
      1,
      11,
      14,
      10,
      9,
      8,
      7,
      0,
      0,
      0,
      0 };

  qsort(&prn, sizeof(prn) / sizeof(prn[0]), sizeof(prn[0]), nmeaQsortPRNCompact);
  CU_ASSERT_EQUAL(memcmp(prn, exp, sizeof(prn)), 0);

  prn1 = 0;
  prn2 = 0;
  r = nmeaQsortPRNCompact(&prn1, &prn2);
  CU_ASSERT_EQUAL(r, 0);

  prn1 = 1;
  prn2 = 0;
  r = nmeaQsortPRNCompact(&prn1, &prn2);
  CU_ASSERT_EQUAL(r, -1);

  prn1 = 0;
  prn2 = 1;
  r = nmeaQsortPRNCompact(&prn1, &prn2);
  CU_ASSERT_EQUAL(r, 1);

  prn1 = 1;
  prn2 = 1;
  r = nmeaQsortPRNCompact(&prn1, &prn2);
  CU_ASSERT_EQUAL(r, 0);

  prn1 = 1;
  prn2 = 2;
  r = nmeaQsortPRNCompact(&prn1, &prn2);
  CU_ASSERT_EQUAL(r, 0);

  prn1 = 2;
  prn2 = 1;
  r = nmeaQsortPRNCompact(&prn1, &prn2);
  CU_ASSERT_EQUAL(r, 0);
}

static void test_nmeaQsortSatelliteCompare(void) {
  NmeaSatellite prn[] = {
      {
          2,
          0,
          0,
          0 },
      {
          3,
          0,
          0,
          0 },
      {
          4,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 },
      {
          1,
          0,
          0,
          0 },
      {
          11,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 },
      {
          14,
          0,
          0,
          0 },
      {
          10,
          0,
          0,
          0 },
      {
          9,
          0,
          0,
          0 },
      {
          8,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 },
      {
          7,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 } };
  NmeaSatellite exp[] = {
      {
          1,
          0,
          0,
          0 },
      {
          2,
          0,
          0,
          0 },
      {
          3,
          0,
          0,
          0 },
      {
          4,
          0,
          0,
          0 },
      {
          7,
          0,
          0,
          0 },
      {
          8,
          0,
          0,
          0 },
      {
          9,
          0,
          0,
          0 },
      {
          10,
          0,
          0,
          0 },
      {
          11,
          0,
          0,
          0 },
      {
          14,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 } };

  qsort(&prn, sizeof(prn) / sizeof(prn[0]), sizeof(prn[0]), nmeaQsortSatelliteCompare);
  CU_ASSERT_EQUAL(memcmp(prn, exp, sizeof(prn)), 0);
}

static void test_nmeaQsortSatelliteCompact(void) {
  NmeaSatellite prn[] = {
      {
          2,
          0,
          0,
          0 },
      {
          3,
          0,
          0,
          0 },
      {
          4,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 },
      {
          1,
          0,
          0,
          0 },
      {
          11,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 },
      {
          14,
          0,
          0,
          0 },
      {
          10,
          0,
          0,
          0 },
      {
          9,
          0,
          0,
          0 },
      {
          8,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 },
      {
          7,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 } };
  NmeaSatellite exp[] = {
      {
          2,
          0,
          0,
          0 },
      {
          3,
          0,
          0,
          0 },
      {
          4,
          0,
          0,
          0 },
      {
          1,
          0,
          0,
          0 },
      {
          11,
          0,
          0,
          0 },
      {
          14,
          0,
          0,
          0 },
      {
          10,
          0,
          0,
          0 },
      {
          9,
          0,
          0,
          0 },
      {
          8,
          0,
          0,
          0 },
      {
          7,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 },
      {
          0,
          0,
          0,
          0 } };

  qsort(&prn, sizeof(prn) / sizeof(prn[0]), sizeof(prn[0]), nmeaQsortSatelliteCompact);
  CU_ASSERT_EQUAL(memcmp(prn, exp, sizeof(prn)), 0);
}

/*
 * Setup
 */

int infoSuiteSetup(void) {
  CU_pSuite pSuite = CU_add_suite("info", mockContextSuiteInit, mockContextSuiteClean);
  if (!pSuite) {
    return CU_get_error();
  }

  if ( //
      (!CU_add_test(pSuite, "nmeaInfoSigToString", test_nmeaInfoSigToString)) //
      || (!CU_add_test(pSuite, "nmeaInfoModeToSig", test_nmeaInfoModeToSig)) //
      || (!CU_add_test(pSuite, "nmeaInfoSigToMode", test_nmeaInfoSigToMode)) //
      || (!CU_add_test(pSuite, "nmeaInfoFixToString", test_nmeaInfoFixToString)) //
      || (!CU_add_test(pSuite, "nmeaInfoFieldToString", test_nmeaInfoFieldToString)) //
      || (!CU_add_test(pSuite, "nmeaInfoIsPresentAll", test_nmeaInfoIsPresentAll)) //
      || (!CU_add_test(pSuite, "nmeaInfoIsPresentAny", test_nmeaInfoIsPresentAny)) //
      || (!CU_add_test(pSuite, "nmeaInfoSetPresent", test_nmeaInfoSetPresent)) //
      || (!CU_add_test(pSuite, "nmeaInfoUnsetPresent", test_nmeaInfoUnsetPresent)) //
      || (!CU_add_test(pSuite, "nmeaTimeParseTime", test_nmeaTimeParseTime)) //
      || (!CU_add_test(pSuite, "nmeaTimeParseDate", test_nmeaTimeParseDate)) //
      || (!CU_add_test(pSuite, "nmeaTimeSet", test_nmeaTimeSet)) //
      || (!CU_add_test(pSuite, "nmeaInfoClear", test_nmeaInfoClear)) //
      || (!CU_add_test(pSuite, "nmeaInfoSanitise", test_nmeaInfoSanitise)) //
      || (!CU_add_test(pSuite, "nmeaInfoUnitConversion", test_nmeaInfoUnitConversion)) //
      || (!CU_add_test(pSuite, "nmeaQsortPRNCompare", test_nmeaQsortPRNCompare)) //
      || (!CU_add_test(pSuite, "nmeaQsortPRNCompact", test_nmeaQsortPRNCompact)) //
      || (!CU_add_test(pSuite, "nmeaQsortSatelliteCompare", test_nmeaQsortSatelliteCompare)) //
      || (!CU_add_test(pSuite, "nmeaQsortSatelliteCompact", test_nmeaQsortSatelliteCompact)) //
      ) {
    return CU_get_error();
  }

  return CUE_SUCCESS;
}

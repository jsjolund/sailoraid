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

#include <nmealib/sentence.h>
#include <CUnit/Basic.h>
#include <float.h>
#include <stdlib.h>

int sentenceSuiteSetup(void);

/*
 * Tests
 */

static void test_nmeaSentenceToPrefix(void) {
  const char *r;

  r = nmeaSentenceToPrefix(NMEALIB_SENTENCE_FIRST - 1);
  CU_ASSERT_PTR_NULL(r);

  r = nmeaSentenceToPrefix(NMEALIB_SENTENCE_GPNON);
  CU_ASSERT_PTR_NULL(r);

  r = nmeaSentenceToPrefix(NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_STRING_EQUAL(r, NMEALIB_GPGGA_PREFIX);

  r = nmeaSentenceToPrefix(NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_STRING_EQUAL(r, NMEALIB_GPGSA_PREFIX);

  r = nmeaSentenceToPrefix(NMEALIB_SENTENCE_GPGSV);
  CU_ASSERT_STRING_EQUAL(r, NMEALIB_GPGSV_PREFIX);

  r = nmeaSentenceToPrefix(NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_STRING_EQUAL(r, NMEALIB_GPRMC_PREFIX);

  r = nmeaSentenceToPrefix(NMEALIB_SENTENCE_GPVTG);
  CU_ASSERT_STRING_EQUAL(r, NMEALIB_GPVTG_PREFIX);

  r = nmeaSentenceToPrefix(NMEALIB_SENTENCE_LAST + 1);
  CU_ASSERT_PTR_NULL(r);
}

static void test_nmeaSentenceFromPrefix(void) {
  NmeaSentence r;
  const char *s;

  s = NULL;
  r = nmeaSentenceFromPrefix(s, 1);
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPNON);

  s = "";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPNON);

  s = "$GPGG";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPNON);

  s = "$GPGGA,blah";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPGGA);

  s = "GPGGA,blah";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPGGA);

  s = "$GPGSA,blah";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPGSA);

  s = "GPGSA,blah";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPGSA);

  s = "$GPGSV,blah";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPGSV);

  s = "GPGSV,blah";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPGSV);

  s = "$GPRMC,blah";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPRMC);

  s = "GPRMC,blah";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPRMC);

  s = "$GPVTG,blah";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPVTG);

  s = "GPVTG,blah";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPVTG);

  s = "$UNKNW,blah";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPNON);

  s = "UNKNW,blah";
  r = nmeaSentenceFromPrefix(s, strlen(s));
  CU_ASSERT_EQUAL(r, NMEALIB_SENTENCE_GPNON);

}

static void test_nmeaSentenceToInfo(void) {
  NmeaInfo infoEmpty;
  NmeaInfo info;
  const char *s;
  bool r;

  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* NULL sentence */

  r = nmeaSentenceToInfo(NULL, 1, &info);
  CU_ASSERT_EQUAL(r, false);
  validatePackToInfo(&info, 0, 0, true);
  memset(&info, 0, sizeof(info));

  /* invalid sentence */

  s = "$GPXXX,blah";
  r = nmeaSentenceToInfo(s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, false);
  validatePackToInfo(&info, 0, 0, true);
  memset(&info, 0, sizeof(info));

  /* GPGGA */

  s = "$GPGGA,invalid";
  r = nmeaSentenceToInfo(s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, false);
  validatePackToInfo(&info, 1, 1, true);
  memset(&info, 0, sizeof(info));

  s = "$GPGGA,104559.64,,,,,,,,,,,,,";
  r = nmeaSentenceToInfo(s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, true);
  validatePackToInfo(&info, 1, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_UTCTIME | NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_EQUAL(info.utc.hour, 10);
  CU_ASSERT_EQUAL(info.utc.min, 45);
  CU_ASSERT_EQUAL(info.utc.sec, 59);
  CU_ASSERT_EQUAL(info.utc.hsec, 64);
  memset(&info, 0, sizeof(info));

  /* GPGSA */

  s = "$GPGSA,invalid";
  r = nmeaSentenceToInfo(s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, false);
  validatePackToInfo(&info, 1, 1, true);
  memset(&info, 0, sizeof(info));

  s = "$GPGSA,,3,,,,,,,,,,,,,,,";
  r = nmeaSentenceToInfo(s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, true);
  validatePackToInfo(&info, 1, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_FIX | NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_EQUAL(info.fix, NMEALIB_FIX_3D);
  memset(&info, 0, sizeof(info));

  /* GPGSV */

  s = "$GPGSV,invalid";
  r = nmeaSentenceToInfo(s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, false);
  validatePackToInfo(&info, 1, 1, true);
  memset(&info, 0, sizeof(info));

  s = "$GPGSV,1,1,4,11,,,45,,,,,12,13,,,,,,";
  r = nmeaSentenceToInfo(s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, true);
  validatePackToInfo(&info, 1, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW | NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSV);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 4);
  CU_ASSERT_EQUAL(info.satellites.inView[0].prn, 11);
  CU_ASSERT_EQUAL(info.satellites.inView[0].elevation, 0);
  CU_ASSERT_EQUAL(info.satellites.inView[0].azimuth, 0);
  CU_ASSERT_EQUAL(info.satellites.inView[0].snr, 45);
  checkSatellitesEmpty(info.satellites.inView, 1, 1, 0);
  CU_ASSERT_EQUAL(info.satellites.inView[2].prn, 12);
  CU_ASSERT_EQUAL(info.satellites.inView[2].elevation, 13);
  CU_ASSERT_EQUAL(info.satellites.inView[2].azimuth, 0);
  CU_ASSERT_EQUAL(info.satellites.inView[2].snr, 0);
  checkSatellitesEmpty(info.satellites.inView, 3, 3, 0);
  memset(&info, 0, sizeof(info));

  /* GPRMC */

  s = "$GPRMC,invalid";
  r = nmeaSentenceToInfo(s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, false);
  validatePackToInfo(&info, 1, 1, true);
  memset(&info, 0, sizeof(info));

  s = "$GPRMC,104559.64,,,,,,,,,,,";
  r = nmeaSentenceToInfo(s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, true);
  validatePackToInfo(&info, 1, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_UTCTIME | NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_EQUAL(info.utc.hour, 10);
  CU_ASSERT_EQUAL(info.utc.min, 45);
  CU_ASSERT_EQUAL(info.utc.sec, 59);
  CU_ASSERT_EQUAL(info.utc.hsec, 64);
  memset(&info, 0, sizeof(info));

  /* GPVTG */

  s = "$GPVTG,1,q";
  r = nmeaSentenceToInfo(s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, false);
  validatePackToInfo(&info, 1, 1, true);
  memset(&info, 0, sizeof(info));

  s = "$GPVTG,,,,,,,4.25,k";
  r = nmeaSentenceToInfo(s, strlen(s), &info);
  CU_ASSERT_EQUAL(r, true);
  validatePackToInfo(&info, 1, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SPEED | NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPVTG);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 4.25, FLT_EPSILON);
  memset(&info, 0, sizeof(info));
}

static void test_nmeaSentenceFromInfo(void) {
  size_t r;
  NmeaInfo infoEmpty;
  NmeaInfo info;
  NmeaMallocedBuffer buf;

  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  r = nmeaSentenceFromInfo(NULL, &info, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);
  memset(&info, 0, sizeof(info));

  buf.buffer = (char *) &info;
  buf.bufferSize = 0;
  r = nmeaSentenceFromInfo(&buf, &info, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);
  CU_ASSERT_PTR_EQUAL(buf.buffer, (char *) &info);
  CU_ASSERT_EQUAL(buf.bufferSize, 0);
  memset(&info, 0, sizeof(info));

  buf.buffer = NULL;
  buf.bufferSize = 1;
  r = nmeaSentenceFromInfo(&buf, &info, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);
  CU_ASSERT_PTR_EQUAL(buf.buffer, NULL);
  CU_ASSERT_EQUAL(buf.bufferSize, 1);
  memset(&info, 0, sizeof(info));

  /* allocate the buffer */
  buf.bufferSize = NMEALIB_BUFFER_CHUNK_SIZE;
  buf.buffer = malloc(buf.bufferSize);
  CU_ASSERT_PTR_NOT_NULL_FATAL(buf.buffer);

  r = nmeaSentenceFromInfo(&buf, NULL, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);
  CU_ASSERT_PTR_NOT_NULL(buf.buffer);
  CU_ASSERT_EQUAL(buf.bufferSize, NMEALIB_BUFFER_CHUNK_SIZE);
  memset(&info, 0, sizeof(info));

  r = nmeaSentenceFromInfo(&buf, &info, 0);
  CU_ASSERT_EQUAL(r, 0);
  validateContext(0, 0);
  CU_ASSERT_PTR_NOT_NULL(buf.buffer);
  CU_ASSERT_EQUAL(buf.bufferSize, NMEALIB_BUFFER_CHUNK_SIZE);
  memset(&info, 0, sizeof(info));

  /* GPGGA */

  info.utc.hour = 12;
  info.utc.min = 22;
  info.utc.sec = 32;
  info.utc.hsec = 42;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_UTCTIME);
  r = nmeaSentenceFromInfo(&buf, &info, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_EQUAL(r, 34);
  CU_ASSERT_PTR_NOT_NULL(buf.buffer);
  CU_ASSERT_EQUAL(buf.bufferSize, NMEALIB_BUFFER_CHUNK_SIZE);
  CU_ASSERT_STRING_EQUAL(buf.buffer, "$GPGGA,122232.42,,,,,,,,,,,,,*7C\r\n");
  validateContext(0, 0);
  memset(&info, 0, sizeof(info));

  /* GPGSA */

  info.fix = NMEALIB_FIX_3D;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_FIX);
  r = nmeaSentenceFromInfo(&buf, &info, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_EQUAL(r, 29);
  CU_ASSERT_PTR_NOT_NULL(buf.buffer);
  CU_ASSERT_EQUAL(buf.bufferSize, NMEALIB_BUFFER_CHUNK_SIZE);
  CU_ASSERT_STRING_EQUAL(buf.buffer, "$GPGSA,,3,,,,,,,,,,,,,,,*5D\r\n");
  validateContext(0, 0);
  memset(&info, 0, sizeof(info));

  /* GPGSV */

  info.satellites.inViewCount = 5;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);
  r = nmeaSentenceFromInfo(&buf, &info, NMEALIB_SENTENCE_GPGSV);
  CU_ASSERT_EQUAL(r, 54);
  CU_ASSERT_PTR_NOT_NULL(buf.buffer);
  CU_ASSERT_EQUAL(buf.bufferSize, NMEALIB_BUFFER_CHUNK_SIZE);
  CU_ASSERT_STRING_EQUAL(buf.buffer, "$GPGSV,2,1,5,,,,,,,,,,,,,,,,*4F\r\n$GPGSV,2,2,5,,,,*4C\r\n");
  validateContext(0, 0);
  memset(&info, 0, sizeof(info));

  /* GPRMC */

  info.utc.hour = 12;
  info.utc.min = 22;
  info.utc.sec = 32;
  info.utc.hsec = 42;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_UTCTIME);
  r = nmeaSentenceFromInfo(&buf, &info, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_EQUAL(r, 32);
  CU_ASSERT_PTR_NOT_NULL(buf.buffer);
  CU_ASSERT_EQUAL(buf.bufferSize, NMEALIB_BUFFER_CHUNK_SIZE);
  CU_ASSERT_STRING_EQUAL(buf.buffer, "$GPRMC,122232.42,,,,,,,,,,,*61\r\n");
  validateContext(0, 0);
  memset(&info, 0, sizeof(info));

  /* GPVTG */

  info.speed = 42.43;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SPEED);
  r = nmeaSentenceFromInfo(&buf, &info, NMEALIB_SENTENCE_GPVTG);
  CU_ASSERT_EQUAL(r, 29);
  CU_ASSERT_PTR_NOT_NULL(buf.buffer);
  CU_ASSERT_EQUAL(buf.bufferSize, NMEALIB_BUFFER_CHUNK_SIZE);
  CU_ASSERT_STRING_EQUAL(buf.buffer, "$GPVTG,,,,,22.9,N,42.4,K*5C\r\n");
  validateContext(0, 0);
  memset(&info, 0, sizeof(info));

  /* invalid mask */

  info.speed = 42.43;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SPEED);
  r = nmeaSentenceFromInfo(&buf, &info, NMEALIB_SENTENCE_LAST << 1);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_PTR_NOT_NULL(buf.buffer);
  CU_ASSERT_EQUAL(buf.bufferSize, NMEALIB_BUFFER_CHUNK_SIZE);
  validateContext(0, 0);
  memset(&info, 0, sizeof(info));

  /* free the buffer */
  free(buf.buffer);
  buf.buffer = NULL;
  buf.bufferSize = 0;

  /* GPVTG */

  info.speed = 42.43;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SPEED);
  r = nmeaSentenceFromInfo(&buf, &info, NMEALIB_SENTENCE_GPVTG);
  CU_ASSERT_EQUAL(r, 29);
  CU_ASSERT_PTR_NOT_NULL(buf.buffer);
  CU_ASSERT_EQUAL(buf.bufferSize, NMEALIB_BUFFER_CHUNK_SIZE);
  CU_ASSERT_STRING_EQUAL(buf.buffer, "$GPVTG,,,,,22.9,N,42.4,K*5C\r\n");
  validateContext(0, 0);
  memset(&info, 0, sizeof(info));

  /* free the buffer */
  free(buf.buffer);
  buf.buffer = NULL;
  buf.bufferSize = 0;
}

/*
 * Setup
 */

int sentenceSuiteSetup(void) {
  CU_pSuite pSuite = CU_add_suite("sentence", mockContextSuiteInit, mockContextSuiteClean);
  if (!pSuite) {
    return CU_get_error();
  }

  if ( //
      (!CU_add_test(pSuite, "nmeaSentenceToPrefix", test_nmeaSentenceToPrefix)) //
      || (!CU_add_test(pSuite, "nmeaSentenceFromPrefix", test_nmeaSentenceFromPrefix)) //
      || (!CU_add_test(pSuite, "nmeaSentenceToInfo", test_nmeaSentenceToInfo)) //
      || (!CU_add_test(pSuite, "nmeaSentenceFromInfo", test_nmeaSentenceFromInfo)) //
      ) {
    return CU_get_error();
  }

  return CUE_SUCCESS;
}

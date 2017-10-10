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

#include <nmealib/gpvtg.h>
#include <nmealib/nmath.h>
#include <nmealib/sentence.h>
#include <CUnit/Basic.h>
#include <float.h>
#include <string.h>

int gpvtgSuiteSetup(void);

/*
 * Tests
 */

static void test_nmeaGPVTGParse(void) {
  const char * s = "some string";
  NmeaGPVTG packEmpty;
  NmeaGPVTG pack;
  bool r;

  memset(&packEmpty, 0, sizeof(packEmpty));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  r = nmeaGPVTGParse(NULL, 1, &pack);
  validateParsePack(&pack, r, false, 0, 0, true);

  r = nmeaGPVTGParse(s, 0, &pack);
  validateParsePack(&pack, r, false, 0, 0, true);

  r = nmeaGPVTGParse(s, strlen(s), NULL);
  validateParsePack(&pack, r, false, 0, 0, true);

  /* invalid sentence / not enough fields */

  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  /* all fields empty */

  s = "$GPVTG,,,,,,,,";
  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, true);

  /* track */

  s = "$GPVTG,4.25,,,,,,,";
  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPVTG,4.25,q,,,,,,";
  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPVTG,4.25,t,,,,,,";
  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_TRACK);
  CU_ASSERT_DOUBLE_EQUAL(pack.track, 4.25, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.trackT, 'T');

  /* mtrack */

  s = "$GPVTG,,,4.25,,,,,";
  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPVTG,,,4.25,q,,,,";
  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPVTG,,,4.25,m,,,,";
  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_MTRACK);
  CU_ASSERT_DOUBLE_EQUAL(pack.mtrack, 4.25, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.mtrackM, 'M');

  /* speed knots */

  s = "$GPVTG,,,,,4.25,,,";
  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPVTG,,,,,4.25,q,,";
  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPVTG,,,,,4.25,n,,";
  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SPEED);
  CU_ASSERT_DOUBLE_EQUAL(pack.spn, 4.25, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.spnN, 'N');
  CU_ASSERT_DOUBLE_EQUAL(pack.spk, (4.25 * NMEALIB_KNOT_TO_KPH), FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.spkK, 'K');

  /* speed kph */

  s = "$GPVTG,,,,,,,4.25,";
  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPVTG,,,,,,,4.25,q";
  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPVTG,,,,,,,4.25,k";
  r = nmeaGPVTGParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SPEED);
  CU_ASSERT_DOUBLE_EQUAL(pack.spk, 4.25, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.spkK, 'K');
  CU_ASSERT_DOUBLE_EQUAL(pack.spn, (4.25 * NMEALIB_KPH_TO_KNOT), FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.spnN, 'N');
}

static void test_nmeaGPVTGToInfo(void) {
  NmeaGPVTG pack;
  NmeaInfo infoEmpty;
  NmeaInfo info;

  memset(&pack, 0, sizeof(pack));
  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  nmeaGPVTGToInfo(NULL, &info);
  validatePackToInfo(&info, 0, 0, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  nmeaGPVTGToInfo(&pack, NULL);
  validatePackToInfo(&info, 0, 0, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* empty */

  nmeaGPVTGToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPVTG);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* track */

  pack.track = 42.75;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_TRACK);

  nmeaGPVTGToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_TRACK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPVTG);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 42.75, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* mtrack */

  pack.mtrack = 42.75;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_MTRACK);

  nmeaGPVTGToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_MTRACK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPVTG);
  CU_ASSERT_DOUBLE_EQUAL(info.mtrack, 42.75, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* speed */

  pack.spn = 42.75;
  pack.spnN = '\0';
  pack.spk = 10.0;
  pack.spkK = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SPEED);

  nmeaGPVTGToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SPEED);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPVTG);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, (42.75 * NMEALIB_KNOT_TO_KPH), FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.spn = 42.75;
  pack.spnN = 'N';
  pack.spk = 10.0;
  pack.spkK = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SPEED);

  nmeaGPVTGToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SPEED);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPVTG);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, (42.75 * NMEALIB_KNOT_TO_KPH), FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.spn = 42.75;
  pack.spnN = '\0';
  pack.spk = 10.0;
  pack.spkK = 'K';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SPEED);

  nmeaGPVTGToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SPEED);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPVTG);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 10.0, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.spn = 42.75;
  pack.spnN = 'N';
  pack.spk = 10.0;
  pack.spkK = 'K';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SPEED);

  nmeaGPVTGToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SPEED);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPVTG);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, 10.0, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));
}

static void test_nmeaGPVTGFromInfo(void) {
  NmeaInfo info;
  NmeaGPVTG packEmpty;
  NmeaGPVTG pack;

  memset(&info, 0, sizeof(info));
  memset(&packEmpty, 0, sizeof(packEmpty));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  nmeaGPVTGFromInfo(NULL, &pack);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  nmeaGPVTGFromInfo(&info, NULL);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  /* empty */

  nmeaGPVTGFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  /* track */

  info.track = 1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_TRACK);

  nmeaGPVTGFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_TRACK);
  CU_ASSERT_DOUBLE_EQUAL(pack.track, 1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.trackT, 'T');
  memset(&info, 0, sizeof(info));

  /* mtrack */

  info.mtrack = 1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_MTRACK);

  nmeaGPVTGFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_MTRACK);
  CU_ASSERT_DOUBLE_EQUAL(pack.mtrack, 1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.mtrackM, 'M');
  memset(&info, 0, sizeof(info));

  /* speed */

  info.speed = 10.0;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SPEED);

  nmeaGPVTGFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SPEED);
  CU_ASSERT_DOUBLE_EQUAL(pack.spn, (10.0 * NMEALIB_KPH_TO_KNOT), FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.spnN, 'N');
  CU_ASSERT_DOUBLE_EQUAL(pack.spk, 10.0, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.spkK, 'K');
  memset(&info, 0, sizeof(info));
}

static void test_nmeaGPVTGGenerate(void) {
  char buf[256];
  NmeaGPVTG pack;
  size_t r;

  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  r = nmeaGPVTGGenerate(NULL, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  r = nmeaGPVTGGenerate(buf, sizeof(buf), NULL);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* empty with 0 length */

  r = nmeaGPVTGGenerate(buf, 0, &pack);
  CU_ASSERT_EQUAL(r, 19);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* empty */

  r = nmeaGPVTGGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 19);
  CU_ASSERT_STRING_EQUAL(buf, "$GPVTG,,,,,,,,*52\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* track */

  pack.track = 42.6;
  pack.trackT = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_TRACK);

  r = nmeaGPVTGGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 23);
  CU_ASSERT_STRING_EQUAL(buf, "$GPVTG,42.6,,,,,,,*4C\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.track = 42.6;
  pack.trackT = 'T';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_TRACK);

  r = nmeaGPVTGGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 24);
  CU_ASSERT_STRING_EQUAL(buf, "$GPVTG,42.6,T,,,,,,*18\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* mtrack */

  pack.mtrack = 42.6;
  pack.mtrackM = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_MTRACK);

  r = nmeaGPVTGGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 23);
  CU_ASSERT_STRING_EQUAL(buf, "$GPVTG,,,42.6,,,,,*4C\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.mtrack = 42.6;
  pack.mtrackM = 'M';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_MTRACK);

  r = nmeaGPVTGGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 24);
  CU_ASSERT_STRING_EQUAL(buf, "$GPVTG,,,42.6,M,,,,*01\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* speed knots */

  pack.spn = 42.6;
  pack.spnN = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SPEED);

  r = nmeaGPVTGGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 26);
  CU_ASSERT_STRING_EQUAL(buf, "$GPVTG,,,,,42.6,,0.0,*62\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.spn = 42.6;
  pack.spnN = 'N';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SPEED);

  r = nmeaGPVTGGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 27);
  CU_ASSERT_STRING_EQUAL(buf, "$GPVTG,,,,,42.6,N,0.0,*2C\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* speed kph */

  pack.spk = 42.6;
  pack.spkK = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SPEED);

  r = nmeaGPVTGGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 26);
  CU_ASSERT_STRING_EQUAL(buf, "$GPVTG,,,,,0.0,,42.6,*62\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.spk = 42.6;
  pack.spkK = 'K';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SPEED);

  r = nmeaGPVTGGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 27);
  CU_ASSERT_STRING_EQUAL(buf, "$GPVTG,,,,,0.0,,42.6,K*29\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));
}

/*
 * Setup
 */

int gpvtgSuiteSetup(void) {
  CU_pSuite pSuite = CU_add_suite("gpvtg", mockContextSuiteInit, mockContextSuiteClean);
  if (!pSuite) {
    return CU_get_error();
  }

  if ( //
      (!CU_add_test(pSuite, "nmeaGPVTGParse", test_nmeaGPVTGParse)) //
      || (!CU_add_test(pSuite, "nmeaGPVTGToInfo", test_nmeaGPVTGToInfo)) //
      || (!CU_add_test(pSuite, "nmeaGPVTGFromInfo", test_nmeaGPVTGFromInfo)) //
      || (!CU_add_test(pSuite, "nmeaGPVTGGenerate", test_nmeaGPVTGGenerate)) //
      ) {
    return CU_get_error();
  }

  return CUE_SUCCESS;
}

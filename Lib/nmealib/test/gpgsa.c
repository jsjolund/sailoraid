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

#include <nmealib/gpgsa.h>
#include <nmealib/sentence.h>
#include <CUnit/Basic.h>
#include <float.h>
#include <string.h>

int gpgsaSuiteSetup(void);

/*
 * Tests
 */

static void test_nmeaGPGSAParse(void) {
  const char * s = "some string";
  NmeaGPGSA packEmpty;
  NmeaGPGSA pack;
  bool r;

  memset(&packEmpty, 0, sizeof(packEmpty));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  r = nmeaGPGSAParse(NULL, 1, &pack);
  validateParsePack(&pack, r, false, 0, 0, true);

  r = nmeaGPGSAParse(s, 0, &pack);
  validateParsePack(&pack, r, false, 0, 0, true);

  r = nmeaGPGSAParse(s, strlen(s), NULL);
  validateParsePack(&pack, r, false, 0, 0, true);

  packEmpty.fix = NMEALIB_FIX_BAD;

  /* invalid sentence / not enough fields */

  r = nmeaGPGSAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  /* all fields empty */

  s = "$GPGSA,,,,,,,,,,,,,,,,,";
  r = nmeaGPGSAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, true);

  /* sig */

  s = "$GPGSA,!,,,,,,,,,,,,,,,,";
  r = nmeaGPGSAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGSA,a,,,,,,,,,,,,,,,,";
  r = nmeaGPGSAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(pack.sig, 'A');

  s = "$GPGSA,m,,,,,,,,,,,,,,,,";
  r = nmeaGPGSAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(pack.sig, 'M');

  /* fix */

  s = "$GPGSA,,42,,,,,,,,,,,,,,,";
  r = nmeaGPGSAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGSA,,3,,,,,,,,,,,,,,,";
  r = nmeaGPGSAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_FIX);
  CU_ASSERT_EQUAL(pack.fix, NMEALIB_FIX_3D);

  /* satPrn */

  s = "$GPGSA,,,12,11,10,5,,7,8,,4,3,2,1,,,";
  r = nmeaGPGSAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SATINUSE);
  CU_ASSERT_EQUAL(pack.prn[0], 12);
  CU_ASSERT_EQUAL(pack.prn[1], 11);
  CU_ASSERT_EQUAL(pack.prn[2], 10);
  CU_ASSERT_EQUAL(pack.prn[3], 5);
  CU_ASSERT_EQUAL(pack.prn[4], 0);
  CU_ASSERT_EQUAL(pack.prn[5], 7);
  CU_ASSERT_EQUAL(pack.prn[6], 8);
  CU_ASSERT_EQUAL(pack.prn[7], 0);
  CU_ASSERT_EQUAL(pack.prn[8], 4);
  CU_ASSERT_EQUAL(pack.prn[9], 3);
  CU_ASSERT_EQUAL(pack.prn[10], 2);
  CU_ASSERT_EQUAL(pack.prn[11], 1);

  /* pdop */

  s = "$GPGSA,,,,,,,,,,,,,,,-12.128,,";
  r = nmeaGPGSAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_PDOP);
  CU_ASSERT_DOUBLE_EQUAL(pack.pdop, 12.128, FLT_EPSILON);

  /* hdop */

  s = "$GPGSA,,,,,,,,,,,,,,,,-12.128,";
  r = nmeaGPGSAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_HDOP);
  CU_ASSERT_DOUBLE_EQUAL(pack.hdop, 12.128, FLT_EPSILON);

  /* vdop */

  s = "$GPGSA,,,,,,,,,,,,,,,,,-12.128";
  r = nmeaGPGSAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_VDOP);
  CU_ASSERT_DOUBLE_EQUAL(pack.vdop, 12.128, FLT_EPSILON);
}

static void test_nmeaGPGSAToInfo(void) {
  size_t i;
  NmeaGPGSA pack;
  NmeaInfo infoEmpty;
  NmeaInfo info;

  memset(&pack, 0, sizeof(pack));
  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  nmeaGPGSAToInfo(NULL, &info);
  validatePackToInfo(&info, 0, 0, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  nmeaGPGSAToInfo(&pack, NULL);
  validatePackToInfo(&info, 0, 0, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* empty */

  nmeaGPGSAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* sig */

  pack.sig = '!';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);
  info.sig = NMEALIB_SIG_FLOAT_RTK;

  nmeaGPGSAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.sig = '!';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);
  info.sig = NMEALIB_SIG_INVALID;

  nmeaGPGSAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_FIX);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.sig = 'A';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);
  info.sig = NMEALIB_SIG_INVALID;

  nmeaGPGSAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_FIX);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.sig = 'M';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);
  info.sig = NMEALIB_SIG_INVALID;

  nmeaGPGSAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_MANUAL);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* fix */

  pack.fix = NMEALIB_FIX_3D;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_FIX);

  nmeaGPGSAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_FIX);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_EQUAL(info.fix, NMEALIB_FIX_3D);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* satPrn */

  pack.prn[0] = 1;
  pack.prn[1] = 2;
  pack.prn[2] = 0;
  pack.prn[3] = 0;
  pack.prn[4] = 5;
  pack.prn[5] = 6;
  pack.prn[6] = 7;
  pack.prn[7] = 8;
  pack.prn[8] = 0;
  pack.prn[9] = 10;
  pack.prn[10] = 11;
  pack.prn[11] = 12;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINUSE);

  nmeaGPGSAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SATINUSECOUNT | NMEALIB_PRESENT_SATINUSE);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);

  CU_ASSERT_EQUAL(info.satellites.inUse[0], 1);
  CU_ASSERT_EQUAL(info.satellites.inUse[1], 2);
  CU_ASSERT_EQUAL(info.satellites.inUse[2], 5);
  CU_ASSERT_EQUAL(info.satellites.inUse[3], 6);
  CU_ASSERT_EQUAL(info.satellites.inUse[4], 7);
  CU_ASSERT_EQUAL(info.satellites.inUse[5], 8);
  CU_ASSERT_EQUAL(info.satellites.inUse[6], 10);
  CU_ASSERT_EQUAL(info.satellites.inUse[7], 11);
  CU_ASSERT_EQUAL(info.satellites.inUse[8], 12);
  for (i = 9; i < NMEALIB_MAX_SATELLITES; i++) {
    CU_ASSERT_EQUAL(info.satellites.inUse[i], 0);
  }

  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* pdop */

  pack.pdop = -1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_PDOP);

  nmeaGPGSAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_PDOP);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.pdop = 1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_PDOP);

  nmeaGPGSAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_PDOP);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_DOUBLE_EQUAL(info.pdop, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* hdop */

  pack.hdop = -1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_HDOP);

  nmeaGPGSAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_HDOP);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.hdop = 1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_HDOP);

  nmeaGPGSAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_HDOP);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* vdop */

  pack.vdop = -1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_VDOP);

  nmeaGPGSAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_VDOP);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.vdop = 1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_VDOP);

  nmeaGPGSAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_VDOP);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSA);
  CU_ASSERT_DOUBLE_EQUAL(info.vdop, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));
}

static void test_nmeaGPGSAFromInfo(void) {
  NmeaInfo info;
  NmeaGPGSA packEmpty;
  NmeaGPGSA pack;

  memset(&info, 0, sizeof(info));
  memset(&packEmpty, 0, sizeof(packEmpty));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  nmeaGPGSAFromInfo(NULL, &pack);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  nmeaGPGSAFromInfo(&info, NULL);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  packEmpty.fix = NMEALIB_FIX_BAD;

  /* empty */

  nmeaGPGSAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  /* sig */

  info.sig = NMEALIB_SIG_ESTIMATED;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SIG);

  nmeaGPGSAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(pack.sig, 'A');
  memset(&info, 0, sizeof(info));

  info.sig = NMEALIB_SIG_MANUAL;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SIG);

  nmeaGPGSAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(pack.sig, 'M');
  memset(&info, 0, sizeof(info));

  /* fix */

  info.fix = NMEALIB_FIX_2D;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_FIX);

  nmeaGPGSAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_FIX);
  CU_ASSERT_EQUAL(pack.fix, NMEALIB_FIX_2D);
  memset(&info, 0, sizeof(info));

  /* satPrn */

  info.satellites.inUse[0] = 1;
  info.satellites.inUse[1] = 2;
  info.satellites.inUse[2] = 5;
  info.satellites.inUse[3] = 6;
  info.satellites.inUse[4] = 7;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SATINUSE);

  nmeaGPGSAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SATINUSE);
  CU_ASSERT_EQUAL(pack.prn[0], 1);
  CU_ASSERT_EQUAL(pack.prn[1], 2);
  CU_ASSERT_EQUAL(pack.prn[2], 5);
  CU_ASSERT_EQUAL(pack.prn[3], 6);
  CU_ASSERT_EQUAL(pack.prn[4], 7);
  CU_ASSERT_EQUAL(pack.prn[5], 0);
  CU_ASSERT_EQUAL(pack.prn[6], 0);
  CU_ASSERT_EQUAL(pack.prn[7], 0);
  CU_ASSERT_EQUAL(pack.prn[8], 0);
  CU_ASSERT_EQUAL(pack.prn[9], 0);
  CU_ASSERT_EQUAL(pack.prn[10], 0);
  CU_ASSERT_EQUAL(pack.prn[11], 0);
  memset(&info, 0, sizeof(info));

  info.satellites.inUse[0] = 1;
  info.satellites.inUse[1] = 2;
  info.satellites.inUse[2] = 5;
  info.satellites.inUse[3] = 6;
  info.satellites.inUse[4] = 7;
  info.satellites.inUse[5] = 9;
  info.satellites.inUse[6] = 10;
  info.satellites.inUse[7] = 11;
  info.satellites.inUse[8] = 14;
  info.satellites.inUse[9] = 15;
  info.satellites.inUse[10] = 16;
  info.satellites.inUse[11] = 17;
  info.satellites.inUse[12] = 18;
  info.satellites.inUse[13] = 19;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SATINUSE);

  nmeaGPGSAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SATINUSE);
  CU_ASSERT_EQUAL(pack.prn[0], 1);
  CU_ASSERT_EQUAL(pack.prn[1], 2);
  CU_ASSERT_EQUAL(pack.prn[2], 5);
  CU_ASSERT_EQUAL(pack.prn[3], 6);
  CU_ASSERT_EQUAL(pack.prn[4], 7);
  CU_ASSERT_EQUAL(pack.prn[5], 9);
  CU_ASSERT_EQUAL(pack.prn[6], 10);
  CU_ASSERT_EQUAL(pack.prn[7], 11);
  CU_ASSERT_EQUAL(pack.prn[8], 14);
  CU_ASSERT_EQUAL(pack.prn[9], 15);
  CU_ASSERT_EQUAL(pack.prn[10], 16);
  CU_ASSERT_EQUAL(pack.prn[11], 17);
  memset(&info, 0, sizeof(info));

  /* pdop */

  info.pdop = 1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_PDOP);

  nmeaGPGSAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_PDOP);
  CU_ASSERT_DOUBLE_EQUAL(pack.pdop, 1232.5523, FLT_EPSILON);
  memset(&info, 0, sizeof(info));

  /* hdop */

  info.hdop = 1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_HDOP);

  nmeaGPGSAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_HDOP);
  CU_ASSERT_DOUBLE_EQUAL(pack.hdop, 1232.5523, FLT_EPSILON);
  memset(&info, 0, sizeof(info));

  /* vdop */

  info.vdop = 1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_VDOP);

  nmeaGPGSAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_VDOP);
  CU_ASSERT_DOUBLE_EQUAL(pack.vdop, 1232.5523, FLT_EPSILON);
  memset(&info, 0, sizeof(info));
}

static void test_nmeaGPGSAGenerate(void) {
  char buf[256];
  NmeaGPGSA pack;
  size_t r;

  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  r = nmeaGPGSAGenerate(NULL, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  r = nmeaGPGSAGenerate(buf, sizeof(buf), NULL);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* empty with 0 length */

  r = nmeaGPGSAGenerate(buf, 0, &pack);
  CU_ASSERT_EQUAL(r, 28);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* empty */

  r = nmeaGPGSAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 28);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGSA,,,,,,,,,,,,,,,,,*6E\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* sig */

  pack.sig = 'A';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);

  r = nmeaGPGSAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 29);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGSA,A,,,,,,,,,,,,,,,,*2F\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.sig = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);

  r = nmeaGPGSAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 28);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGSA,,,,,,,,,,,,,,,,,*6E\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* fix */

  pack.fix = NMEALIB_FIX_3D;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_FIX);

  r = nmeaGPGSAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 29);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGSA,,3,,,,,,,,,,,,,,,*5D\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* satPrn */

  pack.prn[0] = 1;
  pack.prn[1] = 2;
  pack.prn[2] = 3;
  pack.prn[4] = 5;
  pack.prn[5] = 6;
  pack.prn[6] = 7;
  pack.prn[9] = 10;
  pack.prn[10] = 11;
  pack.prn[11] = 12;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINUSE);

  r = nmeaGPGSAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 40);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGSA,,,1,2,3,,5,6,7,,,10,11,12,,,*68\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* pdop */

  pack.pdop = 42.64;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_PDOP);

  r = nmeaGPGSAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 32);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGSA,,,,,,,,,,,,,,,42.6,,*70\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* hdop */

  pack.hdop = 42.64;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_HDOP);

  r = nmeaGPGSAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 32);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGSA,,,,,,,,,,,,,,,,42.6,*70\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* vdop */

  pack.vdop = 42.64;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_VDOP);

  r = nmeaGPGSAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 32);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGSA,,,,,,,,,,,,,,,,,42.6*70\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));
}

/*
 * Setup
 */

int gpgsaSuiteSetup(void) {
  CU_pSuite pSuite = CU_add_suite("gpgsa", mockContextSuiteInit, mockContextSuiteClean);
  if (!pSuite) {
    return CU_get_error();
  }

  if ( //
      (!CU_add_test(pSuite, "nmeaGPGSAParse", test_nmeaGPGSAParse)) //
      || (!CU_add_test(pSuite, "nmeaGPGSAToInfo", test_nmeaGPGSAToInfo)) //
      || (!CU_add_test(pSuite, "nmeaGPGSAFromInfo", test_nmeaGPGSAFromInfo)) //
      || (!CU_add_test(pSuite, "nmeaGPGSAGenerate", test_nmeaGPGSAGenerate)) //
      ) {
    return CU_get_error();
  }

  return CUE_SUCCESS;
}

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

#include <nmealib/gpgsv.h>
#include <nmealib/sentence.h>
#include <CUnit/Basic.h>
#include <stddef.h>

int gpgsvSuiteSetup(void);

/*
 * Tests
 */

static void test_nmeaGPGSVsatellitesToSentencesCount(void) {
  size_t r;

  r = nmeaGPGSVsatellitesToSentencesCount(0);
  CU_ASSERT_EQUAL(r, 1);

  r = nmeaGPGSVsatellitesToSentencesCount(1);
  CU_ASSERT_EQUAL(r, 1);

  r = nmeaGPGSVsatellitesToSentencesCount(2);
  CU_ASSERT_EQUAL(r, 1);

  r = nmeaGPGSVsatellitesToSentencesCount(3);
  CU_ASSERT_EQUAL(r, 1);

  r = nmeaGPGSVsatellitesToSentencesCount(4);
  CU_ASSERT_EQUAL(r, 1);

  r = nmeaGPGSVsatellitesToSentencesCount(5);
  CU_ASSERT_EQUAL(r, 2);
}

static void test_nmeaGPGSVParse(void) {
  const char * s = "some string";
  NmeaGPGSV packEmpty;
  NmeaGPGSV pack;
  bool r;

  memset(&packEmpty, 0, sizeof(packEmpty));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  pack.inViewCount = 10;
  r = nmeaGPGSVParse(NULL, 1, &pack);
  validateParsePack(&pack, r, false, 0, 0, true);

  pack.inViewCount = 10;
  r = nmeaGPGSVParse(s, 0, &pack);
  validateParsePack(&pack, r, false, 0, 0, true);

  r = nmeaGPGSVParse(s, strlen(s), NULL);
  validateParsePack(&pack, r, false, 0, 0, true);

  /* mandatory fields not set */

  s = "$GPGSV,,,,,,,,,,,,,,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 0, true);

  s = "$GPGSV,3,,,,,,,,,,,,,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 0, true);

  s = "$GPGSV,3,2,,,,,,,,,,,,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 0, true);

  /* invalid satellites */

  s = "$GPGSV,250,1,1000,,,,,,,,,,,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  /* invalid sentences */

  s = "$GPGSV,0,2,10,,,,,,,,,,,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGSV,100,2,40,,,,,,,,,,,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGSV,2,2,10,,,,,,,,,,,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  /* invalid sentence */

  s = "$GPGSV,3,0,10,,,,,,,,,,,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGSV,3,4,10,,,,,,,,,,,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  /* not enough fields (1 field too few) */

  s = "$GPGSV,1,1,4,,,,,,,,,,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGSV,3,2,10,,,,,,,,,,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  /* all satellites empty */

  s = "$GPGSV,1,1,4,,,,,,,,,,,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(pack.sentenceCount, 1);
  CU_ASSERT_EQUAL(pack.sentence, 1);
  CU_ASSERT_EQUAL(pack.inViewCount, 4);
  checkSatellitesEmpty(pack.inView, 0, 3, 0);

  /* invalid satellite */

  s = "$GPGSV,1,1,4,11,,,100,,,,,,,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  /* valid satellites, incomplete */

  s = "$GPGSV,1,1,4,11,,,45,,,,,12,13,,,,,,";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(pack.sentenceCount, 1);
  CU_ASSERT_EQUAL(pack.sentence, 1);
  CU_ASSERT_EQUAL(pack.inViewCount, 4);
  CU_ASSERT_EQUAL(pack.inView[0].prn, 11);
  CU_ASSERT_EQUAL(pack.inView[0].elevation, 0);
  CU_ASSERT_EQUAL(pack.inView[0].azimuth, 0);
  CU_ASSERT_EQUAL(pack.inView[0].snr, 45);
  checkSatellitesEmpty(pack.inView, 1, 1, 0);
  CU_ASSERT_EQUAL(pack.inView[2].prn, 12);
  CU_ASSERT_EQUAL(pack.inView[2].elevation, 13);
  CU_ASSERT_EQUAL(pack.inView[2].azimuth, 0);
  CU_ASSERT_EQUAL(pack.inView[2].snr, 0);
  checkSatellitesEmpty(pack.inView, 3, 3, 0);

  /* valid satellites */

  s = "$GPGSV,1,1,4,,,,,,,,,,,,,1,2,3,4";
  r = nmeaGPGSVParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(pack.sentenceCount, 1);
  CU_ASSERT_EQUAL(pack.sentence, 1);
  CU_ASSERT_EQUAL(pack.inViewCount, 4);
  checkSatellitesEmpty(pack.inView, 0, 2, 0);
  CU_ASSERT_EQUAL(pack.inView[3].prn, 1);
  CU_ASSERT_EQUAL(pack.inView[3].elevation, 2);
  CU_ASSERT_EQUAL(pack.inView[3].azimuth, 3);
  CU_ASSERT_EQUAL(pack.inView[3].snr, 4);
}

static void test_nmeaGPGSVToInfo(void) {
  NmeaGPGSV pack;
  NmeaInfo infoEmpty;
  NmeaInfo info;

  memset(&pack, 0, sizeof(pack));
  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  nmeaGPGSVToInfo(NULL, &info);
  validatePackToInfo(&info, 0, 0, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  nmeaGPGSVToInfo(&pack, NULL);
  validatePackToInfo(&info, 0, 0, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* too many satellites */

  pack.inViewCount = NMEALIB_MAX_SATELLITES + 1;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT);

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 1, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.inViewCount = NMEALIB_MAX_SATELLITES + 1;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEW);

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 1, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.inViewCount = NMEALIB_MAX_SATELLITES + 1;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 1, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* invalid sentences */

  pack.sentenceCount = 0;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEW);

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 1, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.sentenceCount = NMEALIB_GPGSV_MAX_SENTENCES + 1;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEW);

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 1, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.sentenceCount = 5;
  pack.inViewCount = 10;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEW);

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 1, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* invalid sentence */

  pack.sentence = 0;
  pack.sentenceCount = 3;
  pack.inViewCount = 10;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEW);

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 1, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.sentence = 4;
  pack.sentenceCount = 3;
  pack.inViewCount = 10;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEW);

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 1, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* empty */

  memset(info.satellites.inView, 0xaa, sizeof(info.satellites.inView));

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSV);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  CU_ASSERT_EQUAL(info.progress.gpgsvInProgress, false);
  checkSatellitesEmpty(info.satellites.inView, 0, NMEALIB_MAX_SATELLITES - 1, 0xaaaaaaaa);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* only satellite count */

  pack.inViewCount = 12;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT);
  memset(info.satellites.inView, 0xaa, sizeof(info.satellites.inView));

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SATINVIEWCOUNT);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSV);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 12);
  CU_ASSERT_EQUAL(info.progress.gpgsvInProgress, false);
  checkSatellitesEmpty(info.satellites.inView, 0, NMEALIB_MAX_SATELLITES - 1, 0xaaaaaaaa);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* only satellites, only sentence */

  pack.sentence = 1;
  pack.sentenceCount = 1;
  pack.inViewCount = 1;
  pack.inView[0].prn = 11;
  pack.inView[0].elevation = 15;
  pack.inView[0].azimuth = 30;
  pack.inView[0].snr = 45;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEW);
  memset(info.satellites.inView, 0xaa, sizeof(info.satellites.inView));

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSV);
  CU_ASSERT_EQUAL(info.progress.gpgsvInProgress, false);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  CU_ASSERT_EQUAL(info.satellites.inView[0].prn, 11);
  CU_ASSERT_EQUAL(info.satellites.inView[0].elevation, 15);
  CU_ASSERT_EQUAL(info.satellites.inView[0].azimuth, 30);
  CU_ASSERT_EQUAL(info.satellites.inView[0].snr, 45);
  checkSatellitesEmpty(info.satellites.inView, 1, NMEALIB_MAX_SATELLITES - 1, 0);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* only satellites, only sentence and sparse input */

  pack.sentence = 1;
  pack.sentenceCount = 1;
  pack.inViewCount = 2;
  pack.inView[0].prn = 10;
  pack.inView[0].elevation = 1;
  pack.inView[0].azimuth = 3;
  pack.inView[0].snr = 5;
  pack.inView[2].prn = 11;
  pack.inView[2].elevation = 15;
  pack.inView[2].azimuth = 30;
  pack.inView[2].snr = 45;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEW);
  memset(info.satellites.inView, 0xaa, sizeof(info.satellites.inView));

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSV);
  CU_ASSERT_EQUAL(info.progress.gpgsvInProgress, false);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  CU_ASSERT_EQUAL(info.satellites.inView[0].prn, 10);
  CU_ASSERT_EQUAL(info.satellites.inView[0].elevation, 1);
  CU_ASSERT_EQUAL(info.satellites.inView[0].azimuth, 3);
  CU_ASSERT_EQUAL(info.satellites.inView[0].snr, 5);
  checkSatellitesEmpty(info.satellites.inView, 1, 1, 0);
  CU_ASSERT_EQUAL(info.satellites.inView[2].prn, 11);
  CU_ASSERT_EQUAL(info.satellites.inView[2].elevation, 15);
  CU_ASSERT_EQUAL(info.satellites.inView[2].azimuth, 30);
  CU_ASSERT_EQUAL(info.satellites.inView[2].snr, 45);
  checkSatellitesEmpty(info.satellites.inView, 3, NMEALIB_MAX_SATELLITES - 1, 0);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* only satellites, first sentence and sparse input */

  pack.sentence = 1;
  pack.sentenceCount = 2;
  pack.inViewCount = 7;
  pack.inView[2].prn = 11;
  pack.inView[2].elevation = 15;
  pack.inView[2].azimuth = 30;
  pack.inView[2].snr = 45;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEW);
  memset(info.satellites.inView, 0xaa, sizeof(info.satellites.inView));

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSV);
  CU_ASSERT_EQUAL(info.progress.gpgsvInProgress, true);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  checkSatellitesEmpty(info.satellites.inView, 0, 1, 0);
  CU_ASSERT_EQUAL(info.satellites.inView[2].prn, 11);
  CU_ASSERT_EQUAL(info.satellites.inView[2].elevation, 15);
  CU_ASSERT_EQUAL(info.satellites.inView[2].azimuth, 30);
  CU_ASSERT_EQUAL(info.satellites.inView[2].snr, 45);
  checkSatellitesEmpty(info.satellites.inView, 3, NMEALIB_MAX_SATELLITES - 1, 0);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* only satellites, middle sentence and sparse input */

  pack.sentence = 2;
  pack.sentenceCount = 3;
  pack.inViewCount = 10;
  pack.inView[2].prn = 11;
  pack.inView[2].elevation = 15;
  pack.inView[2].azimuth = 30;
  pack.inView[2].snr = 45;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEW);
  memset(info.satellites.inView, 0xaa, sizeof(info.satellites.inView));

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSV);
  CU_ASSERT_EQUAL(info.progress.gpgsvInProgress, true);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  checkSatellitesEmpty(info.satellites.inView, 0, 3, 0xaaaaaaaa);
  checkSatellitesEmpty(info.satellites.inView, 4, 5, 0);
  CU_ASSERT_EQUAL(info.satellites.inView[6].prn, 11);
  CU_ASSERT_EQUAL(info.satellites.inView[6].elevation, 15);
  CU_ASSERT_EQUAL(info.satellites.inView[6].azimuth, 30);
  CU_ASSERT_EQUAL(info.satellites.inView[6].snr, 45);
  checkSatellitesEmpty(info.satellites.inView, 7, NMEALIB_MAX_SATELLITES - 1, 0);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* only satellites, last sentence and sparse input */

  pack.sentence = 3;
  pack.sentenceCount = 3;
  pack.inViewCount = 10;
  pack.inView[2].prn = 11;
  pack.inView[2].elevation = 15;
  pack.inView[2].azimuth = 30;
  pack.inView[2].snr = 45;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEW);
  memset(info.satellites.inView, 0xaa, sizeof(info.satellites.inView));

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSV);
  CU_ASSERT_EQUAL(info.progress.gpgsvInProgress, false);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  checkSatellitesEmpty(info.satellites.inView, 0, 7, 0xaaaaaaaa);
  checkSatellitesEmpty(info.satellites.inView, 8, 9, 0);
  CU_ASSERT_EQUAL(info.satellites.inView[10].prn, 11);
  CU_ASSERT_EQUAL(info.satellites.inView[10].elevation, 15);
  CU_ASSERT_EQUAL(info.satellites.inView[10].azimuth, 30);
  CU_ASSERT_EQUAL(info.satellites.inView[10].snr, 45);
  checkSatellitesEmpty(info.satellites.inView, 11, NMEALIB_MAX_SATELLITES - 1, 0);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* only satellites, last sentence and sparse input at last possible sentence */

  pack.sentence = NMEALIB_GPGSV_MAX_SENTENCES;
  pack.sentenceCount = NMEALIB_GPGSV_MAX_SENTENCES;
  pack.inViewCount = NMEALIB_MAX_SATELLITES;
  pack.inView[2].prn = 11;
  pack.inView[2].elevation = 15;
  pack.inView[2].azimuth = 30;
  pack.inView[2].snr = 45;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEW);
  memset(info.satellites.inView, 0xaa, sizeof(info.satellites.inView));

  nmeaGPGSVToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGSV);
  CU_ASSERT_EQUAL(info.progress.gpgsvInProgress, false);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 0);
  checkSatellitesEmpty(info.satellites.inView, 0, NMEALIB_MAX_SATELLITES - NMEALIB_GPGSV_MAX_SATS_PER_SENTENCE - 1,
      0xaaaaaaaa);
  checkSatellitesEmpty(info.satellites.inView, NMEALIB_MAX_SATELLITES - NMEALIB_GPGSV_MAX_SATS_PER_SENTENCE,
      NMEALIB_MAX_SATELLITES - NMEALIB_GPGSV_MAX_SATS_PER_SENTENCE + 1, 0);
  CU_ASSERT_EQUAL(info.satellites.inView[NMEALIB_MAX_SATELLITES - NMEALIB_GPGSV_MAX_SATS_PER_SENTENCE + 2].prn, 11);
  CU_ASSERT_EQUAL(info.satellites.inView[NMEALIB_MAX_SATELLITES - NMEALIB_GPGSV_MAX_SATS_PER_SENTENCE + 2].elevation,
      15);
  CU_ASSERT_EQUAL(info.satellites.inView[NMEALIB_MAX_SATELLITES - NMEALIB_GPGSV_MAX_SATS_PER_SENTENCE + 2].azimuth, 30);
  CU_ASSERT_EQUAL(info.satellites.inView[NMEALIB_MAX_SATELLITES - NMEALIB_GPGSV_MAX_SATS_PER_SENTENCE + 2].snr, 45);
  checkSatellitesEmpty(info.satellites.inView, NMEALIB_MAX_SATELLITES - NMEALIB_GPGSV_MAX_SATS_PER_SENTENCE + 3,
      NMEALIB_MAX_SATELLITES - 1, 0);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));
}

static void test_nmeaGPGSVFromInfo(void) {
  NmeaInfo info;
  NmeaGPGSV packEmpty;
  NmeaGPGSV pack;

  memset(&info, 0, sizeof(info));
  memset(&packEmpty, 0, sizeof(packEmpty));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  pack.inViewCount = 10;
  nmeaGPGSVFromInfo(NULL, &pack, 0);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  nmeaGPGSVFromInfo(&info, NULL, 0);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  /* empty */

  pack.inViewCount = 10;
  nmeaGPGSVFromInfo(&info, &pack, 0);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  /* no satellites in view */

  pack.inViewCount = 10;
  info.satellites.inViewCount = 0;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SATINVIEWCOUNT);

  nmeaGPGSVFromInfo(&info, &pack, 0);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  /* invalid sentence */

  pack.inViewCount = 10;
  info.satellites.inViewCount = 2;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SATINVIEWCOUNT);

  nmeaGPGSVFromInfo(&info, &pack, 1);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  /* only satellite count */

  info.satellites.inViewCount = 10;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SATINVIEWCOUNT);

  nmeaGPGSVFromInfo(&info, &pack, 1);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT);
  CU_ASSERT_EQUAL(pack.inViewCount, 10);
  CU_ASSERT_EQUAL(pack.sentenceCount, 3);
  CU_ASSERT_EQUAL(pack.sentence, 0);
  checkSatellitesEmpty(pack.inView, 0, NMEALIB_GPGSV_MAX_SATS_PER_SENTENCE - 1, 0);
  memset(&info, 0, sizeof(info));

  /* satellites, first sentence */

  info.satellites.inViewCount = 10;
  info.satellites.inView[0].prn = 1;
  info.satellites.inView[0].elevation = 2;
  info.satellites.inView[0].azimuth = 3;
  info.satellites.inView[0].snr = 4;
  info.satellites.inView[3].prn = 5;
  info.satellites.inView[3].elevation = 6;
  info.satellites.inView[3].azimuth = 7;
  info.satellites.inView[3].snr = 8;
  info.satellites.inView[4].prn = 9;
  info.satellites.inView[4].elevation = 10;
  info.satellites.inView[4].azimuth = 11;
  info.satellites.inView[4].snr = 12;
  info.satellites.inView[9].prn = 13;
  info.satellites.inView[9].elevation = 14;
  info.satellites.inView[9].azimuth = 15;
  info.satellites.inView[10].snr = 16;
  info.satellites.inView[10].prn = 17;
  info.satellites.inView[10].elevation = 18;
  info.satellites.inView[10].azimuth = 19;
  info.satellites.inView[10].snr = 20;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);

  nmeaGPGSVFromInfo(&info, &pack, 0);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(pack.inViewCount, 10);
  CU_ASSERT_EQUAL(pack.sentenceCount, 3);
  CU_ASSERT_EQUAL(pack.sentence, 1);
  CU_ASSERT_EQUAL(pack.inView[0].prn, 1);
  CU_ASSERT_EQUAL(pack.inView[0].elevation, 2);
  CU_ASSERT_EQUAL(pack.inView[0].azimuth, 3);
  CU_ASSERT_EQUAL(pack.inView[0].snr, 4);
  checkSatellitesEmpty(pack.inView, 1, 2, 0);
  CU_ASSERT_EQUAL(pack.inView[3].prn, 5);
  CU_ASSERT_EQUAL(pack.inView[3].elevation, 6);
  CU_ASSERT_EQUAL(pack.inView[3].azimuth, 7);
  CU_ASSERT_EQUAL(pack.inView[3].snr, 8);
  memset(&info, 0, sizeof(info));

  /* satellites, middle sentence */

  info.satellites.inViewCount = 10;
  info.satellites.inView[0].prn = 1;
  info.satellites.inView[0].elevation = 2;
  info.satellites.inView[0].azimuth = 3;
  info.satellites.inView[0].snr = 4;
  info.satellites.inView[3].prn = 5;
  info.satellites.inView[3].elevation = 6;
  info.satellites.inView[3].azimuth = 7;
  info.satellites.inView[3].snr = 8;
  info.satellites.inView[4].prn = 9;
  info.satellites.inView[4].elevation = 10;
  info.satellites.inView[4].azimuth = 11;
  info.satellites.inView[4].snr = 12;
  info.satellites.inView[9].prn = 13;
  info.satellites.inView[9].elevation = 14;
  info.satellites.inView[9].azimuth = 15;
  info.satellites.inView[9].snr = 16;
  info.satellites.inView[10].prn = 17;
  info.satellites.inView[10].elevation = 18;
  info.satellites.inView[10].azimuth = 19;
  info.satellites.inView[10].snr = 20;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);

  nmeaGPGSVFromInfo(&info, &pack, 1);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(pack.inViewCount, 10);
  CU_ASSERT_EQUAL(pack.sentenceCount, 3);
  CU_ASSERT_EQUAL(pack.sentence, 2);
  CU_ASSERT_EQUAL(pack.inView[0].prn, 9);
  CU_ASSERT_EQUAL(pack.inView[0].elevation, 10);
  CU_ASSERT_EQUAL(pack.inView[0].azimuth, 11);
  CU_ASSERT_EQUAL(pack.inView[0].snr, 12);
  checkSatellitesEmpty(pack.inView, 1, 3, 0);
  memset(&info, 0, sizeof(info));

  /* satellites, last sentence */

  info.satellites.inViewCount = 10;
  info.satellites.inView[0].prn = 1;
  info.satellites.inView[0].elevation = 2;
  info.satellites.inView[0].azimuth = 3;
  info.satellites.inView[0].snr = 4;
  info.satellites.inView[3].prn = 5;
  info.satellites.inView[3].elevation = 6;
  info.satellites.inView[3].azimuth = 7;
  info.satellites.inView[3].snr = 8;
  info.satellites.inView[4].prn = 9;
  info.satellites.inView[4].elevation = 10;
  info.satellites.inView[4].azimuth = 11;
  info.satellites.inView[4].snr = 12;
  info.satellites.inView[9].prn = 13;
  info.satellites.inView[9].elevation = 14;
  info.satellites.inView[9].azimuth = 15;
  info.satellites.inView[9].snr = 16;
  info.satellites.inView[10].prn = 17;
  info.satellites.inView[10].elevation = 18;
  info.satellites.inView[10].azimuth = 19;
  info.satellites.inView[10].snr = 20;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);

  nmeaGPGSVFromInfo(&info, &pack, 2);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);
  CU_ASSERT_EQUAL(pack.inViewCount, 10);
  CU_ASSERT_EQUAL(pack.sentenceCount, 3);
  CU_ASSERT_EQUAL(pack.sentence, 3);
  checkSatellitesEmpty(pack.inView, 0, 0, 0);
  CU_ASSERT_EQUAL(pack.inView[1].prn, 13);
  CU_ASSERT_EQUAL(pack.inView[1].elevation, 14);
  CU_ASSERT_EQUAL(pack.inView[1].azimuth, 15);
  CU_ASSERT_EQUAL(pack.inView[1].snr, 16);
  CU_ASSERT_EQUAL(pack.inView[2].prn, 17);
  CU_ASSERT_EQUAL(pack.inView[2].elevation, 18);
  CU_ASSERT_EQUAL(pack.inView[2].azimuth, 19);
  CU_ASSERT_EQUAL(pack.inView[2].snr, 20);
  checkSatellitesEmpty(pack.inView, 3, 3, 0);
  memset(&info, 0, sizeof(info));
}

static void test_nmeaGPGSVGenerate(void) {
  char buf[256];
  NmeaGPGSV pack;
  size_t r;

  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  r = nmeaGPGSVGenerate(NULL, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  r = nmeaGPGSVGenerate(buf, sizeof(buf), NULL);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* empty with 0 length */

  r = nmeaGPGSVGenerate(buf, 0, &pack);
  CU_ASSERT_EQUAL(r, 17);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* empty */

  r = nmeaGPGSVGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 17);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGSV,1,1,0*49\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* only satellite count */

  pack.inViewCount = 10;
  pack.sentenceCount = 7;
  pack.sentence = 2;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT);

  r = nmeaGPGSVGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 18);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGSV,7,1,10*7E\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* only satellites */

  pack.inViewCount = 10;
  pack.sentenceCount = 7;
  pack.sentence = 2;
  pack.inView[0].prn = 10;
  pack.inView[0].elevation = 1;
  pack.inView[0].azimuth = 3;
  pack.inView[0].snr = 5;
  pack.inView[2].prn = 11;
  pack.inView[2].elevation = 15;
  pack.inView[2].azimuth = 30;
  pack.inView[2].snr = 45;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEW);

  r = nmeaGPGSVGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 46);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGSV,1,2,0,10,1,3,5,,,,,11,15,30,45,,,,*7A\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* all */

  pack.inViewCount = 10;
  pack.sentenceCount = 7;
  pack.sentence = 2;
  pack.inView[0].prn = 10;
  pack.inView[0].elevation = 1;
  pack.inView[0].azimuth = 3;
  pack.inView[0].snr = 5;
  pack.inView[2].prn = 11;
  pack.inView[2].elevation = 15;
  pack.inView[2].azimuth = 30;
  pack.inView[2].snr = 45;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT | NMEALIB_PRESENT_SATINVIEW);

  r = nmeaGPGSVGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 47);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGSV,7,2,10,10,1,3,5,,,,,11,15,30,45,,,,*4D\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));
}

/*
 * Setup
 */

int gpgsvSuiteSetup(void) {
  CU_pSuite pSuite = CU_add_suite("gpgsv", mockContextSuiteInit, mockContextSuiteClean);
  if (!pSuite) {
    return CU_get_error();
  }

  if ( //
      (!CU_add_test(pSuite, "nmeaGPGSVsatellitesToSentencesCount", test_nmeaGPGSVsatellitesToSentencesCount)) //
      || (!CU_add_test(pSuite, "nmeaGPGSVParse", test_nmeaGPGSVParse)) //
      || (!CU_add_test(pSuite, "nmeaGPGSVToInfo", test_nmeaGPGSVToInfo)) //
      || (!CU_add_test(pSuite, "nmeaGPGSVFromInfo", test_nmeaGPGSVFromInfo)) //
      || (!CU_add_test(pSuite, "nmeaGPGSVGenerate", test_nmeaGPGSVGenerate)) //
      ) {
    return CU_get_error();
  }

  return CUE_SUCCESS;
}

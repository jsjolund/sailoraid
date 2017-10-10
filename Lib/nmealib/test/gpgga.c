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

#include <nmealib/gpgga.h>
#include <nmealib/sentence.h>
#include <CUnit/Basic.h>
#include <float.h>

int gpggaSuiteSetup(void);

/*
 * Tests
 */

static void test_nmeaGPGGAParse(void) {
  const char * s = "some string";
  NmeaGPGGA packEmpty;
  NmeaGPGGA pack;
  bool r;

  memset(&packEmpty, 0, sizeof(packEmpty));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  r = nmeaGPGGAParse(NULL, 1, &pack);
  validateParsePack(&pack, r, false, 0, 0, true);

  r = nmeaGPGGAParse(s, 0, &pack);
  validateParsePack(&pack, r, false, 0, 0, true);

  r = nmeaGPGGAParse(s, strlen(s), NULL);
  validateParsePack(&pack, r, false, 0, 0, true);

  /* invalid sentence / not enough fields */

  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  /* all fields empty */

  s = "$GPGGA,,,,,,,,,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, true);

  /* time */

  s = "$GPGGA,invalid,,,,,,,,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 0, true);

  s = "$GPGGA,999999,,,,,,,,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGGA,104559.64,,,,,,,,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_UTCTIME);
  CU_ASSERT_EQUAL(pack.utc.hour, 10);
  CU_ASSERT_EQUAL(pack.utc.min, 45);
  CU_ASSERT_EQUAL(pack.utc.sec, 59);
  CU_ASSERT_EQUAL(pack.utc.hsec, 64);

  /* lat */

  s = "$GPGGA,,1,,,,,,,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGGA,,1,!,,,,,,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGGA,,-1242.55,s,,,,,,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_LAT);
  CU_ASSERT_EQUAL(pack.latitude, 1242.55);
  CU_ASSERT_EQUAL(pack.latitudeNS, 'S');

  /* lon */

  s = "$GPGGA,,,,1,,,,,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGGA,,,,1,!,,,,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGGA,,,,-1242.55,e,,,,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_LON);
  CU_ASSERT_EQUAL(pack.longitude, 1242.55);
  CU_ASSERT_EQUAL(pack.longitudeEW, 'E');

  /* sig */

  s = "$GPGGA,,,,,,4242,,,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGGA,,,,,,2,,,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(pack.sig, 2);

  /* satellites */

  s = "$GPGGA,,,,,,,8,,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT);
  CU_ASSERT_EQUAL(pack.inViewCount, 8);

  /* hdop */

  s = "$GPGGA,,,,,,,,-12.128,,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_HDOP);
  CU_ASSERT_DOUBLE_EQUAL(pack.hdop, 12.128, FLT_EPSILON);

  /* elv */

  s = "$GPGGA,,,,,,,,,1,,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGGA,,,,,,,,,1,!,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGGA,,,,,,,,,-42,m,,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_ELV);
  CU_ASSERT_DOUBLE_EQUAL(pack.elevation, -42, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.elevationM, 'M');

  /* diff */

  s = "$GPGGA,,,,,,,,,,,1,,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGGA,,,,,,,,,,,1,!,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPGGA,,,,,,,,,,,-42,m,,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_HEIGHT);
  CU_ASSERT_DOUBLE_EQUAL(pack.height, -42, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.heightM, 'M');

  /* dgpsAge */

  s = "$GPGGA,,,,,,,,,,,,,-1.250,";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_DGPSAGE);
  CU_ASSERT_DOUBLE_EQUAL(pack.dgpsAge, 1.250, FLT_EPSILON);

  /* dgpsSid */

  s = "$GPGGA,,,,,,,,,,,,,,42";
  r = nmeaGPGGAParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_DGPSSID);
  CU_ASSERT_EQUAL(pack.dgpsSid, 42);
}

static void test_nmeaGPGGAToInfo(void) {
  NmeaGPGGA pack;
  NmeaInfo infoEmpty;
  NmeaInfo info;

  memset(&pack, 0, sizeof(pack));
  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  nmeaGPGGAToInfo(NULL, &info);
  validatePackToInfo(&info, 0, 0, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  nmeaGPGGAToInfo(&pack, NULL);
  validatePackToInfo(&info, 0, 0, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* empty */

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* time */

  pack.utc.hour = 12;
  pack.utc.min = 42;
  pack.utc.sec = 43;
  pack.utc.hsec = 44;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_UTCTIME);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_UTCTIME);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_EQUAL(info.utc.hour, 12);
  CU_ASSERT_EQUAL(info.utc.min, 42);
  CU_ASSERT_EQUAL(info.utc.sec, 43);
  CU_ASSERT_EQUAL(info.utc.hsec, 44);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* latitude  */

  pack.latitude = -1232.5523;
  pack.latitudeNS = 'N';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LAT);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.latitude = 1232.5523;
  pack.latitudeNS = 'N';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LAT);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.latitude = -1232.5523;
  pack.latitudeNS = 'S';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LAT);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.latitude = 1232.5523;
  pack.latitudeNS = 'S';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LAT);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* longitude */

  pack.longitude = -1232.5523;
  pack.longitudeEW = 'E';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LON);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));
  memset(&info, 0, sizeof(info));

  pack.longitude = 1232.5523;
  pack.longitudeEW = 'E';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LON);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.longitude = -1232.5523;
  pack.longitudeEW = 'W';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LON);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));

  pack.longitude = 1232.5523;
  pack.longitudeEW = 'W';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LON);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* signal */

  pack.sig = NMEALIB_SIG_FLOAT_RTK;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_FLOAT_RTK);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* satellites */

  pack.inViewCount = 42;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SATINVIEWCOUNT);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_EQUAL(info.satellites.inViewCount, 42);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* hdop */

  pack.hdop = -1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_HDOP);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_HDOP);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.hdop = 1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_HDOP);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_HDOP);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.hdop, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* elv */

  pack.elevation = -1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_ELV);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_ELV);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.elevation = 1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_ELV);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_ELV);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.elevation, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* height */

  pack.height = -1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_HEIGHT);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_HEIGHT);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.height, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.height = 1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_HEIGHT);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_HEIGHT);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.height, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* dgpsAge */

  pack.dgpsAge = -1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_DGPSAGE);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_DGPSAGE);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.dgpsAge = 1232.5523;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_DGPSAGE);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_DGPSAGE);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_DOUBLE_EQUAL(info.dgpsAge, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* dgpsSid */

  pack.dgpsSid = 42;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_DGPSSID);

  nmeaGPGGAToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_DGPSSID);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPGGA);
  CU_ASSERT_EQUAL(info.dgpsSid, 42);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));
}

static void test_nmeaGPGGAFromInfo(void) {
  NmeaInfo info;
  NmeaGPGGA packEmpty;
  NmeaGPGGA pack;

  memset(&info, 0, sizeof(info));
  memset(&packEmpty, 0, sizeof(packEmpty));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  nmeaGPGGAFromInfo(NULL, &pack);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  nmeaGPGGAFromInfo(&info, NULL);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  /* empty */

  nmeaGPGGAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  /* time */

  info.utc.hour = 12;
  info.utc.min = 42;
  info.utc.sec = 43;
  info.utc.hsec = 44;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_UTCTIME);

  nmeaGPGGAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_UTCTIME);
  CU_ASSERT_EQUAL(pack.utc.hour, 12);
  CU_ASSERT_EQUAL(pack.utc.min, 42);
  CU_ASSERT_EQUAL(pack.utc.sec, 43);
  CU_ASSERT_EQUAL(pack.utc.hsec, 44);
  memset(&info, 0, sizeof(info));

  /* latitude  */

  info.latitude = -1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_LAT);

  nmeaGPGGAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_LAT);
  CU_ASSERT_DOUBLE_EQUAL(pack.latitude, 1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.latitudeNS, 'S');
  memset(&info, 0, sizeof(info));

  info.latitude = 1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_LAT);

  nmeaGPGGAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_LAT);
  CU_ASSERT_DOUBLE_EQUAL(pack.latitude, 1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.latitudeNS, 'N');
  memset(&info, 0, sizeof(info));

  /* longitude */

  info.longitude = -1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_LON);

  nmeaGPGGAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_LON);
  CU_ASSERT_DOUBLE_EQUAL(pack.longitude, 1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.longitudeEW, 'W');
  memset(&info, 0, sizeof(info));

  info.longitude = 1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_LON);

  nmeaGPGGAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_LON);
  CU_ASSERT_DOUBLE_EQUAL(pack.longitude, 1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.longitudeEW, 'E');
  memset(&info, 0, sizeof(info));

  /* signal */

  info.sig = NMEALIB_SIG_MANUAL;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SIG);

  nmeaGPGGAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(pack.sig, NMEALIB_SIG_MANUAL);
  memset(&info, 0, sizeof(info));

  /* satellites */

  info.satellites.inViewCount = 42;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SATINVIEWCOUNT);

  nmeaGPGGAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT);
  CU_ASSERT_EQUAL(pack.inViewCount, 42);
  memset(&info, 0, sizeof(info));

  /* hdop */

  info.hdop = 1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_HDOP);

  nmeaGPGGAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_HDOP);
  CU_ASSERT_DOUBLE_EQUAL(pack.hdop, 1232.5523, FLT_EPSILON);
  memset(&info, 0, sizeof(info));

  /* elv */

  info.elevation = -1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_ELV);

  nmeaGPGGAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_ELV);
  CU_ASSERT_DOUBLE_EQUAL(pack.elevation, -1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.elevationM, 'M');
  memset(&info, 0, sizeof(info));

  /* height */

  info.height = -1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_HEIGHT);

  nmeaGPGGAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_HEIGHT);
  CU_ASSERT_DOUBLE_EQUAL(pack.height, -1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.heightM, 'M');
  memset(&info, 0, sizeof(info));

  /* dgpsAge */

  info.dgpsAge = 1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_DGPSAGE);

  nmeaGPGGAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_DGPSAGE);
  CU_ASSERT_DOUBLE_EQUAL(pack.dgpsAge, 1232.5523, FLT_EPSILON);
  memset(&info, 0, sizeof(info));

  /* dgpsSid */

  info.dgpsSid = 42;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_DGPSSID);

  nmeaGPGGAFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_DGPSSID);
  CU_ASSERT_EQUAL(pack.dgpsSid, 42);
  memset(&info, 0, sizeof(info));
}

static void test_nmeaGPGGAGenerate(void) {
  char buf[256];
  NmeaGPGGA pack;
  size_t r;

  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  r = nmeaGPGGAGenerate(NULL, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  r = nmeaGPGGAGenerate(buf, sizeof(buf), NULL);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* empty with 0 length */

  r = nmeaGPGGAGenerate(buf, 0, &pack);
  CU_ASSERT_EQUAL(r, 25);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* empty */

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 25);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,,,,,,,,,,,,,*56\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* time */

  pack.utc.hour = 12;
  pack.utc.min = 42;
  pack.utc.sec = 43;
  pack.utc.hsec = 44;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_UTCTIME);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 34);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,124243.44,,,,,,,,,,,,,*7A\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* latitude */

  pack.latitude = 1242.55;
  pack.latitudeNS = 'N';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 35);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,1242.5500,N,,,,,,,,,,,*33\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.latitude = 1242.55;
  pack.latitudeNS = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 34);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,1242.5500,,,,,,,,,,,,*7D\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* longitude */

  pack.longitude = 1242.55;
  pack.longitudeEW = 'E';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 36);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,,,01242.5500,E,,,,,,,,,*08\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.longitude = 1242.55;
  pack.longitudeEW = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 35);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,,,01242.5500,,,,,,,,,,*4D\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* signal */

  pack.sig = NMEALIB_SIG_MANUAL;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 26);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,,,,,7,,,,,,,,*61\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* satellites */

  pack.inViewCount = 42;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SATINVIEWCOUNT);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 27);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,,,,,,42,,,,,,,*50\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* hdop */

  pack.hdop = 42.64;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_HDOP);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 29);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,,,,,,,42.6,,,,,,*48\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* elv */

  pack.elevation = 42.64;
  pack.elevationM = 'M';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_ELV);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 30);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,,,,,,,,42.6,M,,,,*05\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.elevation = 42.64;
  pack.elevationM = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_ELV);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 29);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,,,,,,,,42.6,,,,,*48\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* height */

  pack.height = 42.64;
  pack.heightM = 'M';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_HEIGHT);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 30);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,,,,,,,,,,42.6,M,,*05\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.height = 42.64;
  pack.heightM = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_HEIGHT);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 29);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,,,,,,,,,,42.6,,,*48\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* dgpsAge */

  pack.dgpsAge = 42.64;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_DGPSAGE);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 29);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,,,,,,,,,,,,42.6,*48\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* dgpsSid */

  pack.dgpsSid = 42;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_DGPSSID);

  r = nmeaGPGGAGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 27);
  CU_ASSERT_STRING_EQUAL(buf, "$GPGGA,,,,,,,,,,,,,,42*50\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));
}

/*
 * Setup
 */

int gpggaSuiteSetup(void) {
  CU_pSuite pSuite = CU_add_suite("gpgga", mockContextSuiteInit, mockContextSuiteClean);
  if (!pSuite) {
    return CU_get_error();
  }

  if ( //
      (!CU_add_test(pSuite, "nmeaGPGGAParse", test_nmeaGPGGAParse)) //
      || (!CU_add_test(pSuite, "nmeaGPGGAToInfo", test_nmeaGPGGAToInfo)) //
      || (!CU_add_test(pSuite, "nmeaGPGGAFromInfo", test_nmeaGPGGAFromInfo)) //
      || (!CU_add_test(pSuite, "nmeaGPGGAGenerate", test_nmeaGPGGAGenerate)) //
      ) {
    return CU_get_error();
  }

  return CUE_SUCCESS;
}

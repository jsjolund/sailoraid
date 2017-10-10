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

#include <nmealib/gprmc.h>
#include <nmealib/nmath.h>
#include <nmealib/sentence.h>
#include <CUnit/Basic.h>
#include <float.h>
#include <string.h>

int gprmcSuiteSetup(void);

/*
 * Tests
 */

static void test_nmeaGPRMCParse(void) {
  const char * s = "some string";
  NmeaGPRMC packEmpty;
  NmeaGPRMC pack;
  bool r;

  memset(&packEmpty, 0, sizeof(packEmpty));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  r = nmeaGPRMCParse(NULL, 1, &pack);
  validateParsePack(&pack, r, false, 0, 0, true);

  r = nmeaGPRMCParse(s, 0, &pack);
  validateParsePack(&pack, r, false, 0, 0, true);

  r = nmeaGPRMCParse(s, strlen(s), NULL);
  validateParsePack(&pack, r, false, 0, 0, true);

  /* invalid sentence / not enough fields */

  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  /* all fields empty */

  s = "$GPRMC,,,,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, true);

  packEmpty.v23 = true;

  s = "$GPRMC,,,,,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, true);

  /* time */

  s = "$GPRMC,invalid,,,,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 0, true);

  s = "$GPRMC,999999,,,,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPRMC,104559.64,,,,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_UTCTIME);
  CU_ASSERT_EQUAL(pack.utc.hour, 10);
  CU_ASSERT_EQUAL(pack.utc.min, 45);
  CU_ASSERT_EQUAL(pack.utc.sec, 59);
  CU_ASSERT_EQUAL(pack.utc.hsec, 64);

  /* sig & mode */

  packEmpty.v23 = false;

  s = "$GPRMC,,!,,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPRMC,,v,,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.v23, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(pack.sigSelection, 'V');
  CU_ASSERT_EQUAL(pack.sig, '\0');

  s = "$GPRMC,,a,,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.v23, false);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(pack.sigSelection, 'A');
  CU_ASSERT_EQUAL(pack.sig, '\0');

  packEmpty.v23 = true;

  s = "$GPRMC,,!,,,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPRMC,,v,,,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, true);

  s = "$GPRMC,,a,,,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, true);

  s = "$GPRMC,,v,,,,,,,,,,!";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPRMC,,a,,,,,,,,,,!";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPRMC,,v,,,,,,,,,,m";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(pack.sigSelection, 'V');
  CU_ASSERT_EQUAL(pack.sig, 'M');

  s = "$GPRMC,,a,,,,,,,,,,m";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(pack.sigSelection, 'A');
  CU_ASSERT_EQUAL(pack.sig, 'M');

  /* lat */

  s = "$GPRMC,,,1,,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPRMC,,,1,!,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPRMC,,,-1242.55,s,,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_LAT);
  CU_ASSERT_EQUAL(pack.latitude, 1242.55);
  CU_ASSERT_EQUAL(pack.latitudeNS, 'S');

  /* lon */

  s = "$GPRMC,,,,,1,,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPRMC,,,,,1,!,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPRMC,,,,,-1242.55,e,,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_LON);
  CU_ASSERT_EQUAL(pack.longitude, 1242.55);
  CU_ASSERT_EQUAL(pack.longitudeEW, 'E');

  /* speed */

  s = "$GPRMC,,,,,,,4.25,,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SPEED);
  CU_ASSERT_DOUBLE_EQUAL(pack.speed, 4.25, FLT_EPSILON);

  /* track */

  s = "$GPRMC,,,,,,,,4.25,,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_TRACK);
  CU_ASSERT_DOUBLE_EQUAL(pack.track, 4.25, FLT_EPSILON);

  /* date */

  s = "$GPRMC,,,,,,,,,invalid,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 0, true);

  s = "$GPRMC,,,,,,,,,999999,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPRMC,,,,,,,,,100516,,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_UTCDATE);
  CU_ASSERT_EQUAL(pack.utc.day, 10);
  CU_ASSERT_EQUAL(pack.utc.mon, 5);
  CU_ASSERT_EQUAL(pack.utc.year, 2016);

  /* magvar */

  s = "$GPRMC,,,,,,,,,,1,,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPRMC,,,,,,,,,,1,!,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, false, 1, 1, true);

  s = "$GPRMC,,,,,,,,,,-1242.55,e,";
  r = nmeaGPRMCParse(s, strlen(s), &pack);
  validateParsePack(&pack, r, true, 1, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_MAGVAR);
  CU_ASSERT_EQUAL(pack.magvar, 1242.55);
  CU_ASSERT_EQUAL(pack.magvarEW, 'E');
}

static void test_nmeaGPRMCToInfo(void) {
  NmeaGPRMC pack;
  NmeaInfo infoEmpty;
  NmeaInfo info;

  memset(&pack, 0, sizeof(pack));
  memset(&infoEmpty, 0, sizeof(infoEmpty));
  memset(&info, 0, sizeof(info));

  /* invalid inputs */

  nmeaGPRMCToInfo(NULL, &info);
  validatePackToInfo(&info, 0, 0, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  nmeaGPRMCToInfo(&pack, NULL);
  validatePackToInfo(&info, 0, 0, true);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* empty */

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* time */

  pack.utc.hour = 12;
  pack.utc.min = 42;
  pack.utc.sec = 43;
  pack.utc.hsec = 44;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_UTCTIME);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_UTCTIME);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_EQUAL(info.utc.hour, 12);
  CU_ASSERT_EQUAL(info.utc.min, 42);
  CU_ASSERT_EQUAL(info.utc.sec, 43);
  CU_ASSERT_EQUAL(info.utc.hsec, 44);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* sig & mode */

  pack.v23 = false;
  pack.sigSelection = '\0';
  pack.sig = 'M';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  info.sig = NMEALIB_SIG_ESTIMATED;
  pack.v23 = false;
  pack.sigSelection = 'A';
  pack.sig = 'M';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.v23 = false;
  pack.sigSelection = 'A';
  pack.sig = 'M';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_FIX);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.v23 = true;
  pack.sigSelection = '\0';
  pack.sig = 'M';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_INVALID);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.v23 = true;
  pack.sigSelection = 'A';
  pack.sig = 'M';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_EQUAL(info.sig, NMEALIB_SIG_MANUAL);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* latitude  */

  pack.latitude = -1232.5523;
  pack.latitudeNS = 'N';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LAT);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.latitude = 1232.5523;
  pack.latitudeNS = 'N';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LAT);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.latitude = -1232.5523;
  pack.latitudeNS = 'S';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LAT);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.latitude = 1232.5523;
  pack.latitudeNS = 'S';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LAT);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* longitude */

  pack.longitude = -1232.5523;
  pack.longitudeEW = 'E';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LON);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.longitude = 1232.5523;
  pack.longitudeEW = 'E';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LON);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.longitude = -1232.5523;
  pack.longitudeEW = 'W';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LON);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.longitude = 1232.5523;
  pack.longitudeEW = 'W';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_LON);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* speed */

  pack.speed = 42.75;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SPEED);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_SPEED);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.speed, (42.75 * NMEALIB_KNOT_TO_KPH), FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* track */

  pack.track = 42.75;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_TRACK);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_TRACK);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.track, 42.75, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* date */

  pack.utc.day = 12;
  pack.utc.mon = 5;
  pack.utc.year = 2016;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_UTCDATE);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_UTCDATE);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_EQUAL(info.utc.day, 12);
  CU_ASSERT_EQUAL(info.utc.mon, 5);
  CU_ASSERT_EQUAL(info.utc.year, 2016);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  /* magvar */

  pack.magvar = -1232.5523;
  pack.magvarEW = 'E';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_MAGVAR);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_MAGVAR);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.magvar = 1232.5523;
  pack.magvarEW = 'E';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_MAGVAR);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_MAGVAR);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.magvar = -1232.5523;
  pack.magvarEW = 'W';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_MAGVAR);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_MAGVAR);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, 1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));

  pack.magvar = 1232.5523;
  pack.magvarEW = 'W';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_MAGVAR);

  nmeaGPRMCToInfo(&pack, &info);
  validatePackToInfo(&info, 0, 0, false);
  CU_ASSERT_EQUAL(info.present, NMEALIB_PRESENT_SMASK | NMEALIB_PRESENT_MAGVAR);
  CU_ASSERT_EQUAL(info.smask, NMEALIB_SENTENCE_GPRMC);
  CU_ASSERT_DOUBLE_EQUAL(info.magvar, -1232.5523, FLT_EPSILON);
  memset(&pack, 0, sizeof(pack));
  memset(&info, 0, sizeof(info));
}

static void test_nmeaGPRMCFromInfo(void) {
  NmeaInfo info;
  NmeaGPRMC packEmpty;
  NmeaGPRMC pack;

  memset(&info, 0, sizeof(info));
  memset(&packEmpty, 0, sizeof(packEmpty));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  nmeaGPRMCFromInfo(NULL, &pack);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  nmeaGPRMCFromInfo(&info, NULL);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  /* empty */

  packEmpty.v23 = true;
  nmeaGPRMCFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, true);
  memset(&info, 0, sizeof(info));

  /* time */

  info.utc.hour = 12;
  info.utc.min = 42;
  info.utc.sec = 43;
  info.utc.hsec = 44;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_UTCTIME);

  nmeaGPRMCFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_UTCTIME);
  CU_ASSERT_EQUAL(pack.utc.hour, 12);
  CU_ASSERT_EQUAL(pack.utc.min, 42);
  CU_ASSERT_EQUAL(pack.utc.sec, 43);
  CU_ASSERT_EQUAL(pack.utc.hsec, 44);
  memset(&info, 0, sizeof(info));

  /* sig & mode */

  info.sig = NMEALIB_SIG_INVALID;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SIG);

  nmeaGPRMCFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(pack.sigSelection, 'V');
  CU_ASSERT_EQUAL(pack.sig, 'N');
  memset(&info, 0, sizeof(info));

  info.sig = NMEALIB_SIG_MANUAL;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SIG);

  nmeaGPRMCFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SIG);
  CU_ASSERT_EQUAL(pack.sigSelection, 'A');
  CU_ASSERT_EQUAL(pack.sig, 'M');
  memset(&info, 0, sizeof(info));

  /* latitude  */

  info.latitude = -1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_LAT);

  nmeaGPRMCFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_LAT);
  CU_ASSERT_DOUBLE_EQUAL(pack.latitude, 1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.latitudeNS, 'S');
  memset(&info, 0, sizeof(info));

  info.latitude = 1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_LAT);

  nmeaGPRMCFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_LAT);
  CU_ASSERT_DOUBLE_EQUAL(pack.latitude, 1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.latitudeNS, 'N');
  memset(&info, 0, sizeof(info));

  /* longitude */

  info.longitude = -1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_LON);

  nmeaGPRMCFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_LON);
  CU_ASSERT_DOUBLE_EQUAL(pack.longitude, 1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.longitudeEW, 'W');
  memset(&info, 0, sizeof(info));

  info.longitude = 1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_LON);

  nmeaGPRMCFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_LON);
  CU_ASSERT_DOUBLE_EQUAL(pack.longitude, 1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.longitudeEW, 'E');
  memset(&info, 0, sizeof(info));

  /* speed */

  info.speed = 42.55;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_SPEED);

  nmeaGPRMCFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_SPEED);
  CU_ASSERT_DOUBLE_EQUAL(pack.speed, (42.55 * NMEALIB_KPH_TO_KNOT), FLT_EPSILON);
  memset(&info, 0, sizeof(info));

  /* track */

  info.track = 1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_TRACK);

  nmeaGPRMCFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_TRACK);
  CU_ASSERT_DOUBLE_EQUAL(pack.track, 1232.5523, FLT_EPSILON);
  memset(&info, 0, sizeof(info));

  /* date */

  info.utc.day = 12;
  info.utc.mon = 5;
  info.utc.year = 2016;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_UTCDATE);

  nmeaGPRMCFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_UTCDATE);
  CU_ASSERT_EQUAL(pack.utc.day, 12);
  CU_ASSERT_EQUAL(pack.utc.mon, 5);
  CU_ASSERT_EQUAL(pack.utc.year, 2016);
  memset(&info, 0, sizeof(info));

  /* magvar */

  info.magvar = -1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_MAGVAR);

  nmeaGPRMCFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_MAGVAR);
  CU_ASSERT_DOUBLE_EQUAL(pack.magvar, 1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.magvarEW, 'W');
  memset(&info, 0, sizeof(info));

  info.longitude = 1232.5523;
  nmeaInfoSetPresent(&info.present, NMEALIB_PRESENT_LON);

  nmeaGPRMCFromInfo(&info, &pack);
  validateInfoToPack(&pack, 0, 0, false);
  CU_ASSERT_EQUAL(pack.v23, true);
  CU_ASSERT_EQUAL(pack.present, NMEALIB_PRESENT_LON);
  CU_ASSERT_DOUBLE_EQUAL(pack.longitude, 1232.5523, FLT_EPSILON);
  CU_ASSERT_EQUAL(pack.longitudeEW, 'E');
  memset(&info, 0, sizeof(info));
}

static void test_nmeaGPRMCGenerate(void) {
  char buf[256];
  NmeaGPRMC pack;
  size_t r;

  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* invalid inputs */

  pack.v23 = false;
  r = nmeaGPRMCGenerate(NULL, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.v23 = true;
  r = nmeaGPRMCGenerate(NULL, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), NULL);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), NULL);
  CU_ASSERT_EQUAL(r, 0);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* empty with 0 length */

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, 0, &pack);
  CU_ASSERT_EQUAL(r, 22);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, 0, &pack);
  CU_ASSERT_EQUAL(r, 23);
  CU_ASSERT_EQUAL(*buf, '\0');
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* empty */

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 22);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,,,,*67\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 23);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,,,,,*4B\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* time */

  pack.utc.hour = 12;
  pack.utc.min = 42;
  pack.utc.sec = 43;
  pack.utc.hsec = 44;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_UTCTIME);

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 31);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,124243.44,,,,,,,,,,*4B\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.utc.hour = 12;
  pack.utc.min = 42;
  pack.utc.sec = 43;
  pack.utc.hsec = 44;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_UTCTIME);

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 32);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,124243.44,,,,,,,,,,,*67\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* sig & mode */

  pack.sigSelection = '\0';
  pack.sig = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 22);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,,,,*67\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.sigSelection = '\0';
  pack.sig = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 23);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,,,,,*4B\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.sigSelection = 'A';
  pack.sig = 'M';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 23);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,A,,,,,,,,,*26\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.sigSelection = 'A';
  pack.sig = 'M';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SIG);

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 25);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,A,,,,,,,,,,M*47\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* latitude */

  pack.latitude = 1242.55;
  pack.latitudeNS = 'N';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 32);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,1242.5500,N,,,,,,,*02\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.latitude = 1242.55;
  pack.latitudeNS = 'N';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 33);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,1242.5500,N,,,,,,,,*2E\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.latitude = 1242.55;
  pack.latitudeNS = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 31);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,1242.5500,,,,,,,,*4C\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.latitude = 1242.55;
  pack.latitudeNS = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LAT);

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 32);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,1242.5500,,,,,,,,,*60\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* longitude */

  pack.longitude = 1242.55;
  pack.longitudeEW = 'E';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 33);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,01242.5500,E,,,,,*39\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.longitude = 1242.55;
  pack.longitudeEW = 'E';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 34);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,01242.5500,E,,,,,,*15\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.longitude = 1242.55;
  pack.longitudeEW = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 32);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,01242.5500,,,,,,*7C\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.longitude = 1242.55;
  pack.longitudeEW = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_LON);

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 33);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,01242.5500,,,,,,,*50\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* speed */

  pack.speed = 42.6;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SPEED);

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 26);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,42.6,,,,*79\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.speed = 42.6;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_SPEED);

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 27);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,42.6,,,,,*55\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* track */

  pack.track = 42.6;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_TRACK);

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 26);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,42.6,,,*79\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.track = 42.6;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_TRACK);

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 27);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,42.6,,,,*55\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* date */

  pack.utc.day = 12;
  pack.utc.mon = 6;
  pack.utc.year = 1990;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_UTCDATE);

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 28);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,,120690,,*6B\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.utc.day = 12;
  pack.utc.mon = 6;
  pack.utc.year = 2016;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_UTCDATE);

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 28);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,,120616,,*65\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.utc.day = 12;
  pack.utc.mon = 6;
  pack.utc.year = 1990;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_UTCDATE);

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 29);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,,120690,,,*47\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.utc.day = 12;
  pack.utc.mon = 6;
  pack.utc.year = 2016;
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_UTCDATE);

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 29);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,,120616,,,*49\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  /* magvar */

  pack.magvar = 1242.55;
  pack.magvarEW = 'E';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_MAGVAR);

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 29);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,,,1242.5,E*3C\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.magvar = 1242.55;
  pack.magvarEW = 'E';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_MAGVAR);

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 30);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,,,1242.5,E,*10\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.magvar = 1242.55;
  pack.magvarEW = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_MAGVAR);

  pack.v23 = false;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 28);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,,,1242.5,*79\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));

  pack.magvar = 1242.55;
  pack.magvarEW = '\0';
  nmeaInfoSetPresent(&pack.present, NMEALIB_PRESENT_MAGVAR);

  pack.v23 = true;
  r = nmeaGPRMCGenerate(buf, sizeof(buf), &pack);
  CU_ASSERT_EQUAL(r, 29);
  CU_ASSERT_STRING_EQUAL(buf, "$GPRMC,,,,,,,,,,1242.5,,*55\r\n");
  memset(buf, 0, sizeof(buf));
  memset(&pack, 0, sizeof(pack));
}

/*
 * Setup
 */

int gprmcSuiteSetup(void) {
  CU_pSuite pSuite = CU_add_suite("gprmc", mockContextSuiteInit, mockContextSuiteClean);
  if (!pSuite) {
    return CU_get_error();
  }

  if ( //
      (!CU_add_test(pSuite, "nmeaGPRMCParse", test_nmeaGPRMCParse)) //
      || (!CU_add_test(pSuite, "nmeaGPRMCToInfo", test_nmeaGPRMCToInfo)) //
      || (!CU_add_test(pSuite, "nmeaGPRMCFromInfo", test_nmeaGPRMCFromInfo)) //
      || (!CU_add_test(pSuite, "nmeaGPRMCGenerate", test_nmeaGPRMCGenerate)) //
      ) {
    return CU_get_error();
  }

  return CUE_SUCCESS;
}

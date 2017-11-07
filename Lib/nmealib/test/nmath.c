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

#include <nmealib/nmath.h>
#include <nmealib/util.h>
#include <CUnit/Basic.h>
#include <float.h>
#include <string.h>

int nmathSuiteSetup(void);

/*
 * Tests
 */

static void test_nmeaMathDegreeToRadian(void) {
  double r;

  r = nmeaMathDegreeToRadian(0.0);
  CU_ASSERT_DOUBLE_EQUAL(r, 0.0, FLT_EPSILON);

  r = nmeaMathDegreeToRadian(180.0);
  CU_ASSERT_DOUBLE_EQUAL(r, NMEALIB_PI, FLT_EPSILON);

  r = nmeaMathDegreeToRadian(45.0);
  CU_ASSERT_DOUBLE_EQUAL(r, NMEALIB_PI / 4.0, FLT_EPSILON);
}

static void test_nmeaMathRadianToDegree(void) {
  double r;

  r = nmeaMathRadianToDegree(0.0);
  CU_ASSERT_DOUBLE_EQUAL(r, 0.0, FLT_EPSILON);

  r = nmeaMathRadianToDegree(NMEALIB_PI);
  CU_ASSERT_DOUBLE_EQUAL(r, 180.0, FLT_EPSILON);

  r = nmeaMathRadianToDegree(NMEALIB_PI / 4.0);
  CU_ASSERT_DOUBLE_EQUAL(r, 45.0, FLT_EPSILON);
}

static void test_nmeaMathNdegToDegree(void) {
  double r;

  r = nmeaMathNdegToDegree(0.0);
  CU_ASSERT_DOUBLE_EQUAL(r, 0.0, FLT_EPSILON);

  r = nmeaMathNdegToDegree(18000.00);
  CU_ASSERT_DOUBLE_EQUAL(r, 180.0, FLT_EPSILON);

  r = nmeaMathNdegToDegree(13015.45);
  CU_ASSERT_DOUBLE_EQUAL(r, 130.257500000000021600499167107045650482, FLT_EPSILON);
}

static void test_nmeaMathDegreeToNdeg(void) {
  double r;

  r = nmeaMathDegreeToNdeg(0.0);
  CU_ASSERT_DOUBLE_EQUAL(r, 0.0, FLT_EPSILON);

  r = nmeaMathDegreeToNdeg(180.0);
  CU_ASSERT_DOUBLE_EQUAL(r, 18000.0, FLT_EPSILON);

  r = nmeaMathDegreeToNdeg(130.2575);
  CU_ASSERT_DOUBLE_EQUAL(r, 13015.449999999998908606357872486114501953, FLT_EPSILON);
}

static void test_nmeaMathNdegToRadian(void) {
  double r;

  r = nmeaMathNdegToRadian(0.0);
  CU_ASSERT_DOUBLE_EQUAL(r, nmeaMathDegreeToRadian(0.0), FLT_EPSILON);

  r = nmeaMathNdegToRadian(18000.00);
  CU_ASSERT_DOUBLE_EQUAL(r, nmeaMathDegreeToRadian(180.0), FLT_EPSILON);

  r = nmeaMathNdegToRadian(13015.45);
  CU_ASSERT_DOUBLE_EQUAL(r, 2.273422250416514245330290577840059996, FLT_EPSILON);
}

static void test_nmeaMathRadianToNdeg(void) {
  double r;

  r = nmeaMathRadianToNdeg(nmeaMathDegreeToRadian(0.0));
  CU_ASSERT_DOUBLE_EQUAL(r, 0.0, FLT_EPSILON);

  r = nmeaMathRadianToNdeg(nmeaMathDegreeToRadian(180.0));
  CU_ASSERT_DOUBLE_EQUAL(r, 18000.0, FLT_EPSILON);

  r = nmeaMathRadianToNdeg(nmeaMathDegreeToRadian(130.2575));
  CU_ASSERT_DOUBLE_EQUAL(r, 13015.449999999998908606357872486114501953, FLT_EPSILON);
}

static void test_nmeaMathPdopCalculate(void) {
  double r;

  r = nmeaMathPdopCalculate(1.0, 1.0);
  CU_ASSERT_DOUBLE_EQUAL(r, sqrt(2.0), FLT_EPSILON);

  r = nmeaMathPdopCalculate(2.0, 2.0);
  CU_ASSERT_DOUBLE_EQUAL(r, sqrt(8.0), FLT_EPSILON);
}

static void test_nmeaMathDopToMeters(void) {
  double r;

  r = nmeaMathDopToMeters(1.0);
  CU_ASSERT_DOUBLE_EQUAL(r, 1.0 * NMEALIB_DOP_TO_METER, FLT_EPSILON);

  r = nmeaMathDopToMeters(2.5);
  CU_ASSERT_DOUBLE_EQUAL(r, 2.5 * NMEALIB_DOP_TO_METER, FLT_EPSILON);
}

static void test_nmeaMathMetersToDop(void) {
  double r;

  r = nmeaMathMetersToDop(1.0);
  CU_ASSERT_DOUBLE_EQUAL(r, 1.0 / NMEALIB_DOP_TO_METER, FLT_EPSILON);

  r = nmeaMathMetersToDop(2.5);
  CU_ASSERT_DOUBLE_EQUAL(r, 2.5 / NMEALIB_DOP_TO_METER, FLT_EPSILON);
}

static void test_nmeaMathInfoToPosition(void) {
  NmeaInfo info;
  NmeaInfo infoExp;
  NmeaPosition pos;
  NmeaPosition posExp;

  /* invalid inputs */

  memset(&info, 0xaa, sizeof(info));
  memset(&pos, 0xaa, sizeof(pos));
  memcpy(&infoExp, &info, sizeof(infoExp));
  memcpy(&posExp, &pos, sizeof(posExp));
  nmeaMathInfoToPosition(NULL, NULL);
  CU_ASSERT_EQUAL(memcmp(&info, &infoExp, sizeof(info)), 0);
  CU_ASSERT_DOUBLE_EQUAL(pos.lat, posExp.lat, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(pos.lon, posExp.lon, FLT_EPSILON);

  /* no info */

  memset(&info, 0xaa, sizeof(info));
  memset(&pos, 0xaa, sizeof(pos));
  memcpy(&infoExp, &info, sizeof(infoExp));
  memcpy(&posExp, &pos, sizeof(posExp));
  posExp.lat = nmeaMathNdegToRadian(NMEALIB_LATITUDE_DEFAULT_NDEG);
  posExp.lon = nmeaMathNdegToRadian(NMEALIB_LONGITUDE_DEFAULT_NDEG);
  nmeaMathInfoToPosition(NULL, &pos);
  CU_ASSERT_EQUAL(memcmp(&info, &infoExp, sizeof(info)), 0);
  CU_ASSERT_DOUBLE_EQUAL(pos.lat, posExp.lat, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(pos.lon, posExp.lon, FLT_EPSILON);

  /* no info position */

  memset(&info, 0xaa, sizeof(info));
  info.present = 0;
  memset(&pos, 0xaa, sizeof(pos));
  memcpy(&infoExp, &info, sizeof(infoExp));
  memcpy(&posExp, &pos, sizeof(posExp));
  posExp.lat = nmeaMathNdegToRadian(NMEALIB_LATITUDE_DEFAULT_NDEG);
  posExp.lon = nmeaMathNdegToRadian(NMEALIB_LONGITUDE_DEFAULT_NDEG);
  nmeaMathInfoToPosition(&info, &pos);
  CU_ASSERT_EQUAL(memcmp(&info, &infoExp, sizeof(info)), 0);
  CU_ASSERT_DOUBLE_EQUAL(pos.lat, posExp.lat, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(pos.lon, posExp.lon, FLT_EPSILON);

  /* info position */

  memset(&info, 0xaa, sizeof(info));
  info.present = NMEALIB_PRESENT_LAT | NMEALIB_PRESENT_LON;
  info.latitude = 12345.67;
  info.longitude = 8910.11;
  memset(&pos, 0xaa, sizeof(pos));
  memcpy(&infoExp, &info, sizeof(infoExp));
  memcpy(&posExp, &pos, sizeof(posExp));
  posExp.lat = nmeaMathNdegToRadian(info.latitude);
  posExp.lon = nmeaMathNdegToRadian(info.longitude);
  nmeaMathInfoToPosition(&info, &pos);
  CU_ASSERT_EQUAL(memcmp(&info, &infoExp, sizeof(info)), 0);
  CU_ASSERT_DOUBLE_EQUAL(pos.lat, posExp.lat, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(pos.lon, posExp.lon, FLT_EPSILON);
}

static void test_nmeaMathPositionToInfo(void) {
  NmeaInfo info;
  NmeaInfo infoExp;
  NmeaPosition pos;
  NmeaPosition posExp;

  /* invalid inputs */

  memset(&pos, 0xaa, sizeof(pos));
  memset(&info, 0xaa, sizeof(info));
  memcpy(&posExp, &pos, sizeof(posExp));
  memcpy(&infoExp, &info, sizeof(infoExp));
  nmeaMathPositionToInfo(NULL, NULL);
  CU_ASSERT_EQUAL(memcmp(&pos, &posExp, sizeof(pos)), 0);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, infoExp.latitude, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, infoExp.longitude, FLT_EPSILON);

  /* no pos */

  memset(&pos, 0xaa, sizeof(pos));
  memset(&info, 0xaa, sizeof(info));
  memcpy(&posExp, &pos, sizeof(posExp));
  memcpy(&infoExp, &info, sizeof(infoExp));
  infoExp.latitude = NMEALIB_LATITUDE_DEFAULT_NDEG;
  infoExp.longitude = NMEALIB_LONGITUDE_DEFAULT_NDEG;
  nmeaMathPositionToInfo(NULL, &info);
  CU_ASSERT_EQUAL(memcmp(&pos, &posExp, sizeof(pos)), 0);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, infoExp.latitude, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, infoExp.longitude, FLT_EPSILON);

  /* pos position */

  memset(&pos, 0xaa, sizeof(pos));
  pos.lat = nmeaMathNdegToRadian(12345.67);
  pos.lon = nmeaMathNdegToRadian(8910.11);
  memset(&info, 0xaa, sizeof(info));
  memcpy(&posExp, &pos, sizeof(posExp));
  memcpy(&infoExp, &info, sizeof(infoExp));
  infoExp.present = NMEALIB_PRESENT_LAT | NMEALIB_PRESENT_LON;
  infoExp.latitude = nmeaMathRadianToNdeg(pos.lat);
  infoExp.longitude = nmeaMathRadianToNdeg(pos.lon);
  nmeaMathPositionToInfo(&pos, &info);
  CU_ASSERT_EQUAL(memcmp(&pos, &posExp, sizeof(pos)), 0);
  CU_ASSERT_DOUBLE_EQUAL(info.latitude, infoExp.latitude, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(info.longitude, infoExp.longitude, FLT_EPSILON);
}

static void test_nmeaMathDistance(void) {
  NmeaPosition from;
  NmeaPosition to;
  double r;

  /* invalid inputs */

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  r = nmeaMathDistance(NULL, &to);
  CU_ASSERT_EQUAL(isNaN(r), true);

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  r = nmeaMathDistance(&from, NULL);
  CU_ASSERT_EQUAL(isNaN(r), true);

  /* same pos */

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  r = nmeaMathDistance(&from, &to);
  CU_ASSERT_DOUBLE_EQUAL(r, 0.0, FLT_EPSILON);

  /* different lat */

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  to.lat = 0.1;
  r = nmeaMathDistance(&from, &to);
  CU_ASSERT_DOUBLE_EQUAL(r, 637813.699999996460974216461181640625, FLT_EPSILON);

  /* different lon */

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  to.lon = 0.1;
  r = nmeaMathDistance(&from, &to);
  CU_ASSERT_DOUBLE_EQUAL(r, 637813.699999996460974216461181640625, FLT_EPSILON);

  /* different lat and lon */

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  to.lat = 0.01;
  to.lon = 0.01;
  r = nmeaMathDistance(&from, &to);
  CU_ASSERT_DOUBLE_EQUAL(r, 90199.726796933086006902158260345458984375, FLT_EPSILON);
}

static void test_nmeaMathDistanceEllipsoid(void) {
  NmeaPosition from;
  NmeaPosition to;
  double fromAzimuth;
  double toAzimuth;
  double r;

  /* invalid inputs */

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  r = nmeaMathDistanceEllipsoid(NULL, &to, NULL, NULL);
  CU_ASSERT_EQUAL(isNaN(r), true);

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  r = nmeaMathDistanceEllipsoid(&from, NULL, NULL, NULL);
  CU_ASSERT_EQUAL(isNaN(r), true);

  /* same pos */

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  r = nmeaMathDistanceEllipsoid(&from, &to, NULL, NULL);
  CU_ASSERT_DOUBLE_EQUAL(r, 0.0, FLT_EPSILON);

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  fromAzimuth = INFINITY;
  toAzimuth = INFINITY;
  r = nmeaMathDistanceEllipsoid(&from, &to, &fromAzimuth, &toAzimuth);
  CU_ASSERT_DOUBLE_EQUAL(r, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(fromAzimuth, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(toAzimuth, 0.0, FLT_EPSILON);

  /* different lat and lon */

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  to.lat = 0.01;
  to.lon = 0.01;
  r = nmeaMathDistanceEllipsoid(&from, &to, NULL, NULL);
  CU_ASSERT_DOUBLE_EQUAL(r, 89596.92102391461958177387714385986328125, FLT_EPSILON);

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  to.lat = 0.01;
  to.lon = -0.03;
  fromAzimuth = INFINITY;
  toAzimuth = INFINITY;
  r = nmeaMathDistanceEllipsoid(&from, &to, &fromAzimuth, &toAzimuth);
  CU_ASSERT_DOUBLE_EQUAL(r, 200881.004138270043767988681793212890625, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(fromAzimuth, -1.2510003836226024631628206407185643911361694335937, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(toAzimuth, -1.25115039620763734973252212512306869029998779296875, FLT_EPSILON);

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  to.lat = 1.0;
  to.lon = 7.0;
  fromAzimuth = INFINITY;
  toAzimuth = INFINITY;
  r = nmeaMathDistanceEllipsoid(&from, &to, &fromAzimuth, &toAzimuth);
  CU_ASSERT_DOUBLE_EQUAL(r, 7297786.762250707484781742095947265625, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(fromAzimuth, 0.40102001168502454664377410153974778950214385986328125, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(toAzimuth, 0.80490629162972171517509423210867680609226226806640625, FLT_EPSILON);
}

static void test_nmeaMathMoveFlat(void) {
  NmeaPosition from;
  NmeaPosition to;
  double azimuth;
  double distance;
  bool r;

  /* invalid inputs */

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  azimuth = 20.0;
  distance = 10.0;
  r = nmeaMathMoveFlat(NULL, &to, azimuth, distance);
  CU_ASSERT_EQUAL(r, false);

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  azimuth = 20.0;
  distance = 10.0;
  r = nmeaMathMoveFlat(&from, NULL, azimuth, distance);
  CU_ASSERT_EQUAL(r, false);

  /* NaN */

  memset(&from, 0, sizeof(from));
  from.lat = 0.0;
  from.lon = NaN;
  memset(&to, 0, sizeof(to));
  azimuth = 20.0;
  distance = 1000.0;
  r = nmeaMathMoveFlat(&from, &to, azimuth, distance);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(isNaN(to.lat), true);
  CU_ASSERT_EQUAL(isNaN(to.lon), true);

  memset(&from, 0, sizeof(from));
  from.lat = NaN;
  from.lon = 0.0;
  memset(&to, 0, sizeof(to));
  azimuth = 20.0;
  distance = 1000.0;
  r = nmeaMathMoveFlat(&from, &to, azimuth, distance);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(isNaN(to.lat), true);
  CU_ASSERT_EQUAL(isNaN(to.lon), true);

  /* normal */

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  azimuth = 20.0;
  distance = 1000.0;
  r = nmeaMathMoveFlat(&from, &to, azimuth, distance);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_DOUBLE_EQUAL(to.lat, 0.14725904972540260207125584202003665268421173095703125, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(to.lon, 0.054014990832735969294997602219154941849410533905029296875, FLT_EPSILON);
}

static void test_nmeaMathMoveFlatEllipsoid(void) {
  NmeaPosition from;
  NmeaPosition to;
  double azimuth;
  double distance;
  double toAzimuth;
  bool r;

  /* invalid inputs */

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  azimuth = 20.0;
  distance = 10.0;
  r = nmeaMathMoveFlatEllipsoid(NULL, &to, azimuth, distance, &toAzimuth);
  CU_ASSERT_EQUAL(r, false);

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  azimuth = 20.0;
  distance = 10.0;
  r = nmeaMathMoveFlatEllipsoid(&from, NULL, azimuth, distance, &toAzimuth);
  CU_ASSERT_EQUAL(r, false);

  /* NaN */

  memset(&from, 0, sizeof(from));
  from.lat = 0.0;
  from.lon = NaN;
  memset(&to, 0, sizeof(to));
  azimuth = 20.0;
  distance = 1000.0;
  r = nmeaMathMoveFlatEllipsoid(&from, &to, azimuth, distance, NULL);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(isNaN(to.lat), true);
  CU_ASSERT_EQUAL(isNaN(to.lon), true);

  memset(&from, 0, sizeof(from));
  from.lat = NaN;
  from.lon = 0.0;
  memset(&to, 0, sizeof(to));
  azimuth = 20.0;
  distance = 1000.0;
  r = nmeaMathMoveFlatEllipsoid(&from, &to, azimuth, distance, &toAzimuth);
  CU_ASSERT_EQUAL(r, false);
  CU_ASSERT_EQUAL(isNaN(to.lat), true);
  CU_ASSERT_EQUAL(isNaN(to.lon), true);
  CU_ASSERT_EQUAL(isNaN(toAzimuth), true);

  /* normal 'no move' */

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  azimuth = 20.0;
  distance = 1e-13;
  r = nmeaMathMoveFlatEllipsoid(&from, &to, azimuth, distance, NULL);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_DOUBLE_EQUAL(to.lat, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(to.lon, 0.0, FLT_EPSILON);

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  azimuth = 20.0;
  distance = 1e-13;
  r = nmeaMathMoveFlatEllipsoid(&from, &to, azimuth, distance, &toAzimuth);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_DOUBLE_EQUAL(to.lat, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(to.lon, 0.0, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(toAzimuth, 20.0, FLT_EPSILON);

  /* normal */

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  azimuth = 20.0;
  distance = 1000.0;
  r = nmeaMathMoveFlatEllipsoid(&from, &to, azimuth, distance, NULL);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_DOUBLE_EQUAL(to.lat, 0.0000646292804558685514545665617980318984336918219923973083496094, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(to.lon, 0.0001436181884782029404955955298461844904522877186536788940429688, FLT_EPSILON);

  memset(&from, 0, sizeof(from));
  memset(&to, 0, sizeof(to));
  azimuth = 20.0;
  distance = 1000.0;
  r = nmeaMathMoveFlatEllipsoid(&from, &to, azimuth, distance, &toAzimuth);
  CU_ASSERT_EQUAL(r, true);
  CU_ASSERT_DOUBLE_EQUAL(to.lat, 0.0000646292804558685514545665617980318984336918219923973083496094, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(to.lon, 0.0001436181884782029404955955298461844904522877186536788940429688, FLT_EPSILON);
  CU_ASSERT_DOUBLE_EQUAL(toAzimuth, 1.15044408310221069058343346114270389080047607421875, FLT_EPSILON);
}

/*
 * Setup
 */

int nmathSuiteSetup(void) {
  CU_pSuite pSuite = CU_add_suite("nmath", mockContextSuiteInit, mockContextSuiteClean);
  if (!pSuite) {
    return CU_get_error();
  }

  if ( //
      (!CU_add_test(pSuite, "nmeaMathDegreeToRadian", test_nmeaMathDegreeToRadian)) //
      || (!CU_add_test(pSuite, "nmeaMathRadianToDegree", test_nmeaMathRadianToDegree)) //
      || (!CU_add_test(pSuite, "nmeaMathNdegToDegree", test_nmeaMathNdegToDegree)) //
      || (!CU_add_test(pSuite, "nmeaMathDegreeToNdeg", test_nmeaMathDegreeToNdeg)) //
      || (!CU_add_test(pSuite, "nmeaMathNdegToRadian", test_nmeaMathNdegToRadian)) //
      || (!CU_add_test(pSuite, "nmeaMathRadianToNdeg", test_nmeaMathRadianToNdeg)) //
      || (!CU_add_test(pSuite, "nmeaMathPdopCalculate", test_nmeaMathPdopCalculate)) //
      || (!CU_add_test(pSuite, "nmeaMathDopToMeters", test_nmeaMathDopToMeters)) //
      || (!CU_add_test(pSuite, "nmeaMathMetersToDop", test_nmeaMathMetersToDop)) //
      || (!CU_add_test(pSuite, "nmeaMathInfoToPosition", test_nmeaMathInfoToPosition)) //
      || (!CU_add_test(pSuite, "nmeaMathPositionToInfo", test_nmeaMathPositionToInfo)) //
      || (!CU_add_test(pSuite, "nmeaMathDistance", test_nmeaMathDistance)) //
      || (!CU_add_test(pSuite, "nmeaMathDistanceEllipsoid", test_nmeaMathDistanceEllipsoid)) //
      || (!CU_add_test(pSuite, "nmeaMathDistanceEllipsoid", test_nmeaMathDistanceEllipsoid)) //
      || (!CU_add_test(pSuite, "nmeaMathMoveFlat", test_nmeaMathMoveFlat)) //
      || (!CU_add_test(pSuite, "nmeaMathMoveFlatEllipsoid", test_nmeaMathMoveFlatEllipsoid)) //
      ) {
    return CU_get_error();
  }

  return CUE_SUCCESS;
}

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

#ifndef __NMEALIB_TEST_MOCK_CONTEXT_H_
#define __NMEALIB_TEST_MOCK_CONTEXT_H_

extern int nmeaTraceCalls;
extern int nmeaErrorCalls;

void mockContextReset(void);

int mockContextSuiteInit(void);
int mockContextSuiteClean(void);

#endif /* __NMEALIB_TEST_MOCK_CONTEXT_H_ */

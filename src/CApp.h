/*
 * CApp.h
 *
 *  Created on: 18.10.2015
 *      Author: michi
 */

#ifndef CAPP_H_
#define CAPP_H_

#include <string>
#include <vector>

#define MAX_LINE_SIZE 25

class CApp
{
public:
	virtual ~CApp();
	virtual std::string getName() = 0;
	virtual int init() = 0;
	virtual std::vector<std::string> getView() = 0;
	virtual void buttonChanged(char buttons[3]) = 0;
	std::string fitStrToLine(std::string str);
};
/*
0 black
1 red
2 green
3 yellow
4 blue
5 magenta
6 turquois
7 white
8 dark blue
9 black
*/

#endif /* CAPP_H_ */

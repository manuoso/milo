//---------------------------------------------------------------------------------------------------------------------
//  MILO
//---------------------------------------------------------------------------------------------------------------------
//  Copyright 2021 Manuel Pérez Jiménez (a.k.a. manuoso) manuperezj@gmail.com
//---------------------------------------------------------------------------------------------------------------------
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
//  and associated documentation files (the "Software"), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify, merge, publish, distribute, 
//  sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial 
//  portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
//  BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
//  OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//---------------------------------------------------------------------------------------------------------------------


#ifndef __MILO_MODULES_LOG_MANAGER_H__
#define __MILO_MODULES_LOG_MANAGER_H__ 1

#include <fstream>
#include <iostream>

#include <string>

#include <mutex>
#include <chrono>

namespace milo{
namespace modules{
namespace logger{

	class LogManager {
	public:
		static void init(const std::string _appName);

		static void close();

		static LogManager* get();

	public:
		void message(const std::string &_msg, const std::string &_tag, bool _useCout = false);
		
		void status(const std::string &_msg, bool _useCout = false);

		void warning(const std::string &_msg, bool _useCout = false);

		void error(const std::string &_msg, bool _useCout = false);

	private:
		LogManager(const std::string _appName);
		~LogManager();

		static LogManager *singleton_;

		std::chrono::high_resolution_clock::time_point initTime_;
		std::ofstream logFile_;
		std::mutex lock_;

	};

}
}
}

#endif

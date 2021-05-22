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


#include "milo/modules/logger/LogManager.hpp"

using namespace std;

namespace milo{
namespace modules{
namespace logger{

	LogManager *LogManager::singleton_ = nullptr;

	//---------------------------------------------------------------------------------------------------------------------
	void LogManager::init(const string _appName) 
	{
		if (!singleton_)
			singleton_ = new LogManager(_appName);
		else
			singleton_->warning("Someone tried to reinitialize the Log Manager");
	}

	//---------------------------------------------------------------------------------------------------------------------
	void LogManager::close()
	{
		delete singleton_;
	}

	//---------------------------------------------------------------------------------------------------------------------
	LogManager * LogManager::get()
	{
		return singleton_;
	}

	//---------------------------------------------------------------------------------------------------------------------
	void LogManager::message(const std::string & _msg, const std::string & _tag, bool _useCout) 
	{
		double timeSpan = std::chrono::duration<double>(chrono::high_resolution_clock::now() - initTime_).count();
		std::string logLine = to_string(timeSpan) + "\t [" + _tag + "] " + _msg + "\n";
		
		lock_.lock();
		logFile_ << logLine;
		logFile_.flush();
		lock_.unlock();
		
		if (_useCout)
		{
			std::cout << logLine;
			std::cout.flush();
		}
	}

	//---------------------------------------------------------------------------------------------------------------------
	void LogManager::status(const std::string & _msg, bool _useCout)
	{
		message(_msg, "STATUS", _useCout);
	}

	//---------------------------------------------------------------------------------------------------------------------
	void LogManager::warning(const std::string & _msg, bool _useCout)
	{
		message(_msg, "WARNING", _useCout);
	}

	//---------------------------------------------------------------------------------------------------------------------
	void LogManager::error(const std::string & _msg, bool _useCout)
	{
		message(_msg, "ERROR", _useCout);
	}

	//---------------------------------------------------------------------------------------------------------------------
	LogManager::LogManager(const std::string _appName) 
	{
		auto now = time(NULL);
        auto actualTime = localtime(&now);
		logFile_.open(_appName + + "_" + std::to_string(actualTime->tm_mday) + "_" + std::to_string(actualTime->tm_mon) + "_" + std::to_string(actualTime->tm_year) + "_" + std::to_string(actualTime->tm_hour) + "_" + std::to_string(actualTime->tm_min) + "_" + std::to_string(actualTime->tm_sec) + ".txt");

		initTime_ = chrono::high_resolution_clock::now();
		status("Initialized log");
	}

	//---------------------------------------------------------------------------------------------------------------------
	LogManager::~LogManager() 
	{
		logFile_.close();
	}

}
}
}

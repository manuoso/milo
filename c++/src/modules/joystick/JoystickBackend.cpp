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


#include "milo/modules/joystick/JoystickBackend.h"

namespace milo{
namespace modules{
namespace joystick{

    using namespace milo::modules::logger;

    //---------------------------------------------------------------------------------------------------------------------
	JoystickBackend::JoystickBackend(bool _useCout) :
		init_(false), fd_(-1), ff_(-1)
	{
		useCout_ = _useCout;
		auto joyName = identifyJoy();
		auto ffName = identifyFF(joyName.c_str());
		init_ = openJoy(joyName, ffName);
	}

    //---------------------------------------------------------------------------------------------------------------------
	JoystickBackend::JoystickBackend(bool _useCout, const std::string &_name) :
			init_(false), fd_(-1), ff_(-1)
	{
		useCout_ = _useCout;
		auto ffName = identifyFF(_name.c_str());
		init_ = openJoy(_name.c_str(), ffName);
	}

    //---------------------------------------------------------------------------------------------------------------------
	JoystickBackend::~JoystickBackend()
	{
		closeJoy();
	}

    //---------------------------------------------------------------------------------------------------------------------
	bool JoystickBackend::isInit()
	{
		return init_;
	}

	//---------------------------------------------------------------------------------------------------------------------
	bool JoystickBackend::closeJoy()
	{
		if ((close(ff_) || close(fd_)) < 0)
			return true;
		else
			return false;
	}

    //---------------------------------------------------------------------------------------------------------------------
	int JoystickBackend::getAxisCount()
	{
		uint8_t axisNumber = 0;
		if (ioctl(fd_, JSIOCGAXES, &axisNumber) >= 0)
			axisCount_ = axisNumber;
		else
			axisCount_ = 0;
		return axisCount_;
	}

    //---------------------------------------------------------------------------------------------------------------------
	int JoystickBackend::getButtonsCount()
	{
		uint8_t buttonsNumber = 0;
		if (ioctl(fd_, JSIOCGBUTTONS, &buttonsNumber) >= 0)
			buttonCount_ = buttonsNumber;
		else
			buttonCount_ = 0;
		return buttonCount_;
	}

    //---------------------------------------------------------------------------------------------------------------------
	void JoystickBackend::setDeadzone(double _deadzone)
	{
		deadzone_ = _deadzone;
		return ;
	}

    //---------------------------------------------------------------------------------------------------------------------
	bool JoystickBackend::setFeedback(struct ff_effect _effect)
	{
		if (ff_ == -1)
			return false;

		joyEffect_.direction = 0;  // down
		joyEffect_.type = FF_RUMBLE;
		joyEffect_.u.rumble.strong_magnitude = _effect.u.rumble.strong_magnitude;
		joyEffect_.u.rumble.weak_magnitude = _effect.u.rumble.weak_magnitude;

		joyEffect_.replay.length = 1000;
		joyEffect_.replay.delay = 0;

		updateEffect_ = true;

		return true;
	}

    //---------------------------------------------------------------------------------------------------------------------
	bool JoystickBackend::setCorrection(std::vector<int> _type, std::vector<int> _prec, std::vector<int> _coef)
	{
		const int nAxis = getAxisCount();
		struct js_corr corr[nAxis];
		int cont = 0;

		if (_type.size() > nAxis)
			return false;

		if ((_coef.size()%4 != 0) || (nAxis*4 != _coef.size()))
			return false;

		for (int i = 0; i < nAxis; i++)
		{
			corr[i].type = _type[i];
			corr[i].prec = _prec[i];

			for(int j = 0; j < 4; j++) 
			{
				corr[i].coef[j] = _coef[j+cont];
			}
			cont += 4;
		}

		if (ioctl(fd_, JSIOCSCORR, &corr) < 0) 
			return false;

		return true;
	}

    //---------------------------------------------------------------------------------------------------------------------
	bool JoystickBackend::readJoy(std::string &_eventType, int &_eventNumber, double &_value)
	{
		FD_ZERO(&set_);
		FD_SET(fd_, &set_);

		if (updateEffect_)
		{
			int ret = ioctl(ff_, EVIOCSFF, &joyEffect_);
			updateEffect_ = false;
		}

		bool result = false;
		if (FD_ISSET(fd_, &set_))
		{
			if (read(fd_, &event_, sizeof(js_event)) == -1 && errno != EAGAIN)
				return false;  // joystick is probably closed

			switch (event_.type)
			{
				case JS_EVENT_BUTTON | JS_EVENT_INIT: // not handled initial button state
					break;
				case JS_EVENT_BUTTON:
				{
					_eventType = "button";
					_eventNumber = event_.number;
					_value = (event_.value ? 1 : 0);

					result = true;
				}
					break;
				case JS_EVENT_AXIS | JS_EVENT_INIT: // not handled initial axis state
					break;
				case JS_EVENT_AXIS:
				{
					_eventType = "axis";
					_eventNumber = event_.number;
					double val = event_.value;

					if (!(event_.type & JS_EVENT_INIT))
					{
						val = event_.value;
						if (val > unscaledDeadzone_)
						{
							val -= unscaledDeadzone_;
						}
						else if (val < -unscaledDeadzone_)
						{
							val += unscaledDeadzone_;
						}
						else
						{
							val = 0;
						}
				
						if (val == 0)
							_value = val * abs(scale_);
						else
							_value = val * scale_;

						result = true;
					}
				}
					break;
				default:
		            LogManager::get()->warning("[JOYSTICK] Unknown event type. Time: " + std::to_string(event_.time) + " Value: " + std::to_string(event_.value) + " Type: " + std::to_string(event_.type) + " Number: " + std::to_string(event_.number), useCout_);
					break;
			}
		}

		return result;
	}

    //---------------------------------------------------------------------------------------------------------------------
	std::string JoystickBackend::identifyJoy()
	{
		const char path[] = "/dev/input"; 
		struct dirent *entry;
		struct stat stat_buf;

		auto dev_dir = opendir(path);
		if (dev_dir == nullptr)
		{
            LogManager::get()->error("[JOYSTICK] Couldn't open /dev/input", useCout_);
			return "";
		}

		while ((entry = readdir(dev_dir)) != nullptr)
		{
			// filter entries
			if (strncmp(entry->d_name, "js", 2) != 0)  // skip device if it's not a joystick
				continue;

			std::string current_path = std::string(path) + "/" + entry->d_name;
			if (stat(current_path.c_str(), &stat_buf) == -1)
				continue;

			if (!S_ISCHR(stat_buf.st_mode))  // input devices are character devices, skip other
				continue;

			// get joystick name
			int joy_fd = open(current_path.c_str(), O_RDONLY);

			if (joy_fd == -1)
				continue;

			char current_joy_name[128];
			if (ioctl(joy_fd, JSIOCGNAME(sizeof(current_joy_name)), current_joy_name) < 0)
				strncpy(current_joy_name, "Unknown", sizeof(current_joy_name));

			close(joy_fd);
			closedir(dev_dir);

			std::string name = current_joy_name; // TODO: DELETE THIS VARIABLE
            LogManager::get()->status("[JOYSTICK] Found Joystick: " + name + " in: " + current_path.c_str(), useCout_);

			return current_path;
		}

		return "";
	}

    //---------------------------------------------------------------------------------------------------------------------
	std::string JoystickBackend::identifyFF(const std::string &_name)
	{
		const char path[] = "/dev/input/by-id";
		struct dirent *entry;

		// the selected joy can be a symlink, but we want the canonical /dev/input/jsX
		char realpath_buf[PATH_MAX];
		char *res = realpath(_name.c_str(), realpath_buf);
		if (res == nullptr)
			return "";

		auto dev_dir = opendir(path);
		if (dev_dir == nullptr)
		{
			std::string error = strerror(errno);
            LogManager::get()->error("[JOYSTICK] Couldn't open /dev/input/by-id Error: " + error, useCout_);
			return "";
		}

		const std::string joy_dev_real(realpath_buf);
		std::string joy_dev_id;

		// first, find the device in /dev/input/by-id that corresponds to the selected joy,

		while ((entry = readdir(dev_dir)) != nullptr)
		{
			res = strstr(entry->d_name, "-joystick");
			// filter entries
			if (res == nullptr)  // skip device if it's not a joystick
				continue;

			const auto current_path = std::string(path) + "/" + entry->d_name;
			res = realpath(current_path.c_str(), realpath_buf);
			if (res == nullptr)
				continue;

			const std::string dev_real(realpath_buf);
			if (dev_real == joy_dev_real)
			{
				// we found the ID device which maps to the selected joy
				joy_dev_id = current_path;
				break;
			}
		}

		// if no corresponding ID device was found, the autodetection won't work
		if (joy_dev_id.empty())
			return "";

		const auto joy_dev_id_prefix = joy_dev_id.substr(0, joy_dev_id.length() - strlen("-joystick"));
		std::string event_dev;

		// iterate through the by-id dir once more, this time finding the -event-joystick file with the
		// same prefix as the ID device we've already found
		dev_dir = opendir(path);
		while ((entry = readdir(dev_dir)) != nullptr)
		{
			res = strstr(entry->d_name, "-event-joystick");
			if (res == nullptr)  // skip device if it's not an event joystick
				continue;

			const auto current_path = std::string(path) + "/" + entry->d_name;
			if (current_path.find(joy_dev_id_prefix) != std::string::npos)
			{
	            LogManager::get()->status("[JOYSTICK] Found force feedback event device:  " + current_path, useCout_);
				event_dev = current_path;
				break;
			}
		}

		return event_dev;
	}

    //---------------------------------------------------------------------------------------------------------------------
	bool JoystickBackend::openJoy(const std::string &_joy, const std::string &_ff)
	{
		if (_joy.empty() || _ff.empty())
		{
			LogManager::get()->error("[JOYSTICK] Joy name or ff name is empty", useCout_);
			return false;
		}

		bool first_fault = true;
		while (true)
		{
			fd_ = open(_joy.c_str(), O_RDONLY);

			if (fd_ != -1)
			{	
				// hack for initial state data
				close(fd_);
				fd_ = open(_joy.c_str(), O_RDONLY);
			}

			if (fd_ != -1)
				break;

			if (first_fault)
			{
				LogManager::get()->error("[JOYSTICK] Couldn't open joystick " + _joy + " Retry every second", useCout_);
				first_fault = false;
			}
			sleep(1.0);
		}

		ff_ = open(_ff.c_str(), O_RDWR);
		if (ff_ == -1)
			return false;

		// Set the gain of the device
		int gain = 100;           // between 0 and 100 
		struct input_event ie; 

		ie.type = EV_FF;
		ie.code = FF_GAIN;
		ie.value = 0xFFFFUL * gain / 100;

		if (write(ff_, &ie, sizeof(ie)) == -1)
		{
			std::string error = strerror(errno);
			LogManager::get()->warning("[JOYSTICK] Couldn't set gain on joystick force feedback, Error: " + error, useCout_);
		}

		memset(&joyEffect_, 0, sizeof(joyEffect_));
		joyEffect_.id = -1;
		joyEffect_.direction = 0;  // down
		joyEffect_.type = FF_RUMBLE;
		joyEffect_.u.rumble.strong_magnitude = 0;
		joyEffect_.u.rumble.weak_magnitude = 0;
		joyEffect_.replay.length = 1000;
		joyEffect_.replay.delay = 0;

		int ret = ioctl(ff_, EVIOCSFF, &joyEffect_);

		deadzone_ = 0.1;
		scale_ = -1. / (1. - deadzone_) / 32767.;
		unscaledDeadzone_ = 32767. * deadzone_;

		updateEffect_ = false;

		return true;
	}

}
}
}

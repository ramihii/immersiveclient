//This class handles sending osg notifications to android

#ifndef __IMMERSIVE__OSGANDROIDNOTIFIER__
#define __IMMERSIVE__OSGANDROIDNOTIFIER__


#include <android/log.h>
#include <osg/Notify>
#include <string>

namespace immersive {

class OSG_EXPORT OsgAndroidNotifier : public osg::NotifyHandler
{
	private:
		std::string _tag;
	public:
		void setTag(std::string tag);
		void notify(osg::NotifySeverity severity, const char *message);
};

}
#endif

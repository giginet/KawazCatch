//
//  AudioUtils.cpp
//  KawazCatch
//
//  Created by giginet on 7/27/14.
//
//

#include "AudioUtils.h"
#include "cocos2d.h"

USING_NS_CC;

std::string AudioUtils::getFileName(const char* filename)
{
    auto app = Application::getInstance();
    auto platform = app->getTargetPlatform();
    if (platform == Application::Platform::OS_ANDROID) {
        return cocos2d::StringUtils::format("%s.ogg", filename);
    } else if (platform == Application::Platform::OS_IPHONE || platform == Application::Platform::OS_IPAD) {
        return cocos2d::StringUtils::format("%s.caf", filename);
    }
    return filename;
}
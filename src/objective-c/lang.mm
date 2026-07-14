/**
 * @file        lang.mm
 * @brief       Objective C imlpementation of apple specific
 *              logic for ui/lang.cpp
 * @author      Justin Scott
 * @date        2026-07-12
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#import <Foundation/Foundation.h>
#import <Foundation/NSDistributedNotificationCenter.h>
#import <Foundation/NSLocale.h>

#include "lang.hpp"


///////////////////////////////////////////////////////////////////////////////
/// Class to recieve the langauge change notification
///
///////////////////////////////////////////////////////////////////////////////
@interface LanguageObserver : NSObject
{
ui::lang::apple::LanguageClientImpl* m_client;  ///< Client PImpl
}

///////////////////////////////////////////////////////////////////////////////
/// Constructor
///
/// @param[in]  client      Client to recieve notification
///
/// @return     Constructed observer
///
///////////////////////////////////////////////////////////////////////////////
- (id) init: (ui::lang::apple::LanguageClientImpl*) client;

///////////////////////////////////////////////////////////////////////////////
/// Handle the language change notification
///
/// @param[in]  notification        Language change notification
///
///////////////////////////////////////////////////////////////////////////////
- (void)languageChanged:(NSNotification *) notification;
@end

namespace ui::lang::apple
{
namespace // anonymous
{

///////////////////////////////////////////////////////////////////////////////
/// Language change implementation class
///
/// @ingroup    PIMPL
///
///////////////////////////////////////////////////////////////////////////////
class LanguageClientImpl
    {
public:    
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  client      Client interface class
    ///
    ///////////////////////////////////////////////////////////////////////////////
    LanguageClientImpl (LanguageClient* client);

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the language change
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void languageChanged () { m_client->onLanguageChanged (); }

private:
    LanguageObserver*   m_observer; ///< Observer to recieve the notification
    LanguageClient*     m_client;   ///< Client to forward the langauge change event to
    };

LanguageClientImpl::LanguageClientImpl (LanguageClient* client) :
    m_client (client)
    {
    m_observer = [[LanguageObserver alloc] init : this];
    }
} // namespace anonymous



LanguageClient::LanguageClient ()
    {
    m_impl = new LanguageClientImpl{ this };
    }

LanguageClient::~LanguageClient ()
    {
    delete m_impl;
    }

std::string systemLocale()
    {
    NSLocale* currentLocale     = [NSLocale currentLocale];
    NSString* localeIdentifier  = [currentLocale localeIdentifier];

    return std::string([localeIdentifier UTF8String]);
    }

} // namespace ui::lang::apple

@implementation LanguageObserver

- (id) init: (ui::lang::apple::LanguageClientImpl*) client
    {
    self = [super init];

    m_client = client;

    [[NSDistributedNotificationCenter defaultCenter] addObserver:self
                                                     selector:@selector(languageChanged:)
                                                     name:@"AppleLanguagePreferencesChangedNotification"
                                                     object:nil];

    return self;
    }

- (void) languageChanged:(NSNotification *) notification
    {
    m_client->languageChanged ();
    }


@end

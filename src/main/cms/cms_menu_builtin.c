/*
 * This file is part of Cleanflight and Betaflight.
 *
 * Cleanflight and Betaflight are free software. You can redistribute
 * this software and/or modify this software under the terms of the
 * GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Cleanflight and Betaflight are distributed in the hope that they
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

//
// Built-in menu contents and support functions
//

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "platform.h"

#ifdef USE_CMS

#include "build/version.h"

#include "cms/cms.h"
#include "cms/cms_types.h"

// Sub menus

#include "cms/cms_menu_imu.h"
#include "cms/cms_menu_blackbox.h"
#include "cms/cms_menu_failsafe.h"
#include "cms/cms_menu_ledstrip.h"
#include "cms/cms_menu_misc.h"
#include "cms/cms_menu_osd.h"
#include "cms/cms_menu_power.h"
#include "cms/cms_menu_saveexit.h"

// VTX supplied menus

#include "cms/cms_menu_vtx_common.h"

#include "drivers/system.h"

#include "config/config.h"

#include "msp/msp_protocol.h" // XXX for FC identification... not available elsewhere

#include "cms_menu_builtin.h"


// Info

static char infoGitRev[GIT_SHORT_REVISION_LENGTH + 1];
static char infoTargetName[] = __TARGET__;

static long cmsx_InfoInit(void)
{
    int i;
    for ( i = 0 ; i < GIT_SHORT_REVISION_LENGTH ; i++) {
        if (shortGitRevision[i] >= 'a' && shortGitRevision[i] <= 'f')
            infoGitRev[i] = shortGitRevision[i] - 'a' + 'A';
        else
            infoGitRev[i] = shortGitRevision[i];
    }

    infoGitRev[i] = 0x0; // Terminate string
    return 0;
}

static const OSD_Entry menuInfoEntries[] = {
    { "--- INFO ---", OME_Label, NULL, NULL, 0 },
    { "FWID", OME_String, NULL, BETAFLIGHT_IDENTIFIER, 0 },
    { "FWVER", OME_String, NULL, FC_VERSION_STRING, 0 },
    { "GITREV", OME_String, NULL, infoGitRev, 0 },
    { "TARGET", OME_String, NULL, infoTargetName, 0 },
    { "BACK", OME_Back, NULL, NULL, 0 },
    { NULL, OME_END, NULL, NULL, 0 }
};

static CMS_Menu menuInfo = {
#ifdef CMS_MENU_DEBUG
    .GUARD_text = "MENUINFO",
    .GUARD_type = OME_MENU,
#endif
    .onEnter = cmsx_InfoInit,
    .onExit = NULL,
    .checkRedirect = NULL,
    .entries = menuInfoEntries
};

// Features

static const OSD_Entry menuFeaturesEntries[] =
{
    {"--- FEATURES ---", OME_Label, NULL, NULL, 0},

#if defined(USE_BLACKBOX)
    {"BLACKBOX", OME_Submenu, cmsMenuChange, &cmsx_menuBlackbox, 0},
#endif
#if defined(USE_VTX_CONTROL)
#if defined(USE_VTX_RTC6705) || defined(USE_VTX_SMARTAUDIO) || defined(USE_VTX_TRAMP)
    {"VTX", OME_Submenu, cmsMenuChange, &cmsx_menuVtxRedirect, 0},
#endif
#endif // VTX_CONTROL
#ifdef USE_LED_STRIP
    {"LED STRIP", OME_Submenu, cmsMenuChange, &cmsx_menuLedstrip, 0},
#endif // LED_STRIP
    {"POWER", OME_Submenu, cmsMenuChange, &cmsx_menuPower, 0},
#ifdef USE_CMS_FAILSAFE_MENU
    {"FAILSAFE", OME_Submenu, cmsMenuChange, &cmsx_menuFailsafe, 0},
#endif
    {"BACK", OME_Back, NULL, NULL, 0},
    {NULL, OME_END, NULL, NULL, 0}
};

static CMS_Menu menuFeatures = {
#ifdef CMS_MENU_DEBUG
    .GUARD_text = "MENUFEATURES",
    .GUARD_type = OME_MENU,
#endif
    .onEnter = NULL,
    .onExit = NULL,
    .checkRedirect = NULL,
    .entries = menuFeaturesEntries,
};

static long cmsx_SaveExitMenu(displayPort_t *pDisplay, const void *ptr)
{
    UNUSED(ptr);

    if (getRebootRequired()) {
        cmsMenuChange(pDisplay, &cmsx_menuSaveExitReboot);
    } else {
        cmsMenuChange(pDisplay, &cmsx_menuSaveExit);
    }
    return 0;
}

// Main

static const OSD_Entry menuMainEntries[] =
{
    {"-- MAIN --",  OME_Label, NULL, NULL, 0},

    {"PROFILE",     OME_Submenu,  cmsMenuChange, &cmsx_menuImu, 0},
    {"FEATURES",    OME_Submenu,  cmsMenuChange, &menuFeatures, 0},
#ifdef USE_OSD
    {"OSD",         OME_Submenu,  cmsMenuChange, &cmsx_menuOsd, 0},
#endif
    {"FC&FW INFO",  OME_Submenu,  cmsMenuChange, &menuInfo, 0},
    {"MISC",        OME_Submenu,  cmsMenuChange, &cmsx_menuMisc, 0},
    {"SAVE/EXIT",   OME_Funcall,  cmsx_SaveExitMenu, NULL, 0},
#ifdef CMS_MENU_DEBUG
    {"ERR SAMPLE",  OME_Submenu,  cmsMenuChange, &menuInfoEntries[0], 0},
#endif

    {NULL,OME_END, NULL, NULL, 0}
};

CMS_Menu menuMain = {
#ifdef CMS_MENU_DEBUG
    .GUARD_text = "MENUMAIN",
    .GUARD_type = OME_MENU,
#endif
    .onEnter = NULL,
    .onExit = NULL,
    .checkRedirect = NULL,
    .entries = menuMainEntries,
};
#endif

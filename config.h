/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static unsigned int borderpx  = 1;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static char font[]          = "Iosevka Nerd Font:style=Italic:pixelsize=14:antialias=true:autohint=true";
static const char *fonts[]          = { font };
static char dmenufont[]       = "Iosevka Nerd Font:style=Italic:pixelsize=14:antialias=true:autohint=true";
/* #include "/home/david/.cache/wal/colors-wal-dwm.h" */
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#222222";
static char selbgcolor[]            = "#222222";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
 };

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {"st", "-n", "spfm", "-g", "144x41", "-e", "nnn", NULL };
const char *spcmd3[] = {"keepassxc", NULL };
const char *spcmd4[] = {"nextcloud", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",       spcmd1},
	{"spnnn",        spcmd2},
	{"keepassxc",    spcmd3},
	{"nextcloud",    spcmd4},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class       instance     title       tags mask     isfloating   monitor */
	{ NULL,		   "spterm",    NULL,       SPTAG(0),     1,           -1 },
	{ NULL,		   "spfm",      NULL,       SPTAG(1),	  1,           -1 },
	{ NULL,		   "keepassxc", NULL,       SPTAG(2),	  1,           -1 },
	{ "nextcloud", NULL,        NULL,       SPTAG(3),	  1,           -1 },
	{ "mpv",       NULL,        NULL,       0,            1,           -1 },
	{ "feh",       NULL,        NULL,       0,            1,           -1 },
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
/* static const char *dmenucmd[] = { "dmenu_run", "-i", "-l", "10", "-m", dmenumon, NULL }; */
static const char *dmenucmd[] = { "/home/david/scripts/dmenu_wal.sh", dmenumon, NULL };
static const char *powercmd[] = { "/home/david/scripts/powermenu_dmenu.sh", dmenumon, NULL };
static const char *termcmd[]  = { "st", NULL };
/* static const char *mutecmd[] = { "pamixer", "-t", NULL }; */
/* static const char *volupcmd[] = { "pamixer", "-i", "1", NULL }; */
/* static const char *volbigupcmd[] = { "pamixer", "-i", "1", NULL }; */
/* static const char *voldowncmd[] = { "pamixer", "-d", "1", NULL }; */
/* static const char *volbigdowncmd[] = { "pamixer", "-d", "1", NULL }; */
static const char *mutecmd[] = { "/home/david/scripts/volume_pamixer.sh", "mute", NULL };
static const char *volupcmd[] = { "/home/david/scripts/volume_pamixer.sh", "up", "1", NULL };
static const char *volbigupcmd[] = { "/home/david/scripts/volume_pamixer.sh", "bigup", "1", NULL };
static const char *voldowncmd[] = { "/home/david/scripts/volume_pamixer.sh", "down", "1", NULL };
static const char *volbigdowncmd[] = { "/home/david/scripts/volume_pamixer.sh", "bigdown", "1", NULL };
static const char *audiotogglecmd[] = { "mpc", "toggle", NULL };
static const char *audionextcmd[] = { "mpc", "next", NULL };
static const char *audioprevcmd[] = { "mpc", "prev", NULL };
static const char *lockcmd[] = { "slock", NULL };
static const char *scriptcmd[] = { "/home/david/scripts/display_config_dmenu.sh", dmenumon, NULL };
static const char *screencmd[] = { "/home/david/scripts/maim.sh", NULL };
static const char *croppedscreencmd[] = { "/home/david/scripts/maim_cropped.sh", NULL };
static const char *cmdz[] = { "firefox", NULL };
static const char *cmdx[] = { "st", NULL };
static const char *cmdv[] = { "libreoffice", NULL };
static const char *cmda[] = { "emacs", NULL };
static const char *cmds[] = { "steam", NULL };
static const char *cmdw[] = { "gnome-system-monitor", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "font",               STRING,  &font },
		{ "dmenufont",          STRING,  &dmenufont },
		{ "color0",             STRING,  &normbgcolor },
		{ "color0",             STRING,  &normbordercolor },
		{ "color7",             STRING,  &normfgcolor },
		{ "color2",             STRING,  &selbgcolor },
		{ "color2",             STRING,  &selbordercolor },
		{ "color0",             STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          		INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	 	FLOAT,   &mfact },
};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_p,      spawn,          {.v = powercmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_f,      fullscreen,     {0} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
    /* custom keybindings */
    { 0,                            XF86XK_AudioMute,          spawn, {.v = mutecmd } },
    { 0,                            XF86XK_AudioRaiseVolume,   spawn, {.v = volupcmd } },
    { ShiftMask,                    XF86XK_AudioRaiseVolume,   spawn, {.v = volbigupcmd } },
    { 0,                            XF86XK_AudioLowerVolume,   spawn, {.v = voldowncmd } },
    { ShiftMask,                    XF86XK_AudioLowerVolume,   spawn, {.v = volbigdowncmd } },
    { 0,                            XF86XK_AudioPlay,          spawn, {.v = audiotogglecmd } },
    { 0,                            XF86XK_AudioPause,         spawn, {.v = audiotogglecmd } },
    { 0,                            XF86XK_AudioNext,          spawn, {.v = audionextcmd } },
    { 0,                            XF86XK_AudioPrev,          spawn, {.v = audioprevcmd } },
    { 0,                            XF86XK_ScreenSaver,        spawn, {.v = lockcmd } },
    { 0,                            XK_Print,                  spawn, {.v = screencmd } },
    { ShiftMask,                    XK_Print,                  spawn, {.v = croppedscreencmd } },
    { MODKEY,                       XK_u,                      spawn, {.v = scriptcmd } },
    { MODKEY,                       XK_z,                      spawn, {.v = cmdz } },
    { MODKEY,                       XK_x,                      spawn, {.v = cmdx } },
    { MODKEY,                       XK_v,                      spawn, {.v = cmdv } },
    { MODKEY,                       XK_a,                      spawn, {.v = cmda } },
    { MODKEY,                       XK_s,                      spawn, {.v = cmds } },
    { MODKEY,                       XK_w,                      spawn, {.v = cmdw } },
	{ MODKEY,            			XK_y,  	   togglescratch,  {.ui = 0 } },
	{ MODKEY,            			XK_r,	   togglescratch,  {.ui = 1 } },
	{ MODKEY,            			XK_c,	   togglescratch,  {.ui = 2 } },
	{ MODKEY,            			XK_e,	   togglescratch,  {.ui = 3 } },
    /*{ 0,                            XF86XK_MonBrightnessUp,   spawn, {.v = brightnessupcmd } },
    { 0,                            XF86XK_MonBrightnessDown, spawn, {.v = brightnessdowncmd } },*/
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

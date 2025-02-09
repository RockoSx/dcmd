/*
Special thanks to:
	maddinat0r - Author of discord-connector. https://github.com/maddinat0r/samp-discord-connector
	Y_Less - Author of y_va. https://github.com/pawn-lang/YSI-Includes
	Zeex - For the inspiration with zcmd. https://github.com/Southclaws/zcmd

DCMD is a command handler that gives you more control in your commands.
	-The default prefix is ! you can change it with the function SetPrefix(string).
		public OnGameModeInit(){
			SetPrefix("!!");
			return 1;
		}
	-To create a command you need to simply create a new function in your gamemode/filterscript with the syntax DCMD:commandname(DCC_Message:message, params[]) {return 1;} (The params are optional.)


https://github.com/RockoSchultz/dcmd
*/

/*------------------------------|
|								|
|	By: RockoSx					|
|	Released: 21/07/2021		|
|	Version: 0.1.1				|
|								|
|-----------------------------*/
#if defined _dcmd_include
	#endinput
#else
	#define _dcmd_include
#endif

#if !defined dcconnector_included
	#tryinclude "discord-connector"
#endif
#if !defined dcconnector_included
	#error Could not find discord-connector
#endif

#if !defined _INC_y_va
	#tryinclude "YSI_Coding/y_va"
#endif

#if !defined _INC_y_va
	#warning It's recommended to use y_va in order to use SendDiscord and SendPrivateMessage with integrated formats.
#endif

#if !defined DCC_MAX_MESSAGE
	#define DCC_MAX_MESSAGE 512
#endif

#if !defined DCC_MAX_OLDMSG
	#define DCC_MAX_OLDMSG 12
#endif

#if !defined DCMD_PREFIX_LENGHT
	#define DCMD_PREFIX_LENGHT 6
#endif

#define INVALID_MESSAGE DCC_Message:0

#define DCMD:%1(%2)			\
forward dcmd_ % 1( % 2);
\
public dcmd_ % 1( % 2)

//--------------------------------------------------
forward OnCreatedPrivateChannel(const message[]); // You may not use this callback, use SendPrivateMessage instead.
forward OnDiscordCommandReceived(const cmdtext[], DCC_Message:message); // This callback y called first when a command is received.
forward OnDiscordCommandPerformed(const cmdtext[], sucess, DCC_Message:message); // This callback is called when a command is evaluated, meanly when the function of cmdtext is called (e.g: dcmd_verifydiscord), if doesn't exist sucess = 1.

static DCC_Message:oldMessage,
       prefix[DCMD_PREFIX_LENGHT] = "!", // You may need to change the initial parameter for prefix, default !
                                    bool:AllowBotsCommands = false,
                                    bool:dcmd_hasODCR = false,
                                    bool:dcmd_hasODCP = false;

//--------------------------------------------------

//Utility

#if defined _INC_y_va
stock SendDiscord(DCC_Channel:channel, const text[], {Float, _} :...)
{
    new msg[512];
    va_format(msg, 512, text, va_start<2>);
    DCC_SendChannelMessage(channel, msg);
    return 1;
}
#else
stock SendDiscord(DCC_Channel:channel, const text[])
{
    return DCC_SendChannelMessage(channel, text);
}
#endif

#if defined _INC_y_va
stock SendPrivateMessage(DCC_User:user, const message[], {Float, _} :...)
{
    new msg[1024];
    va_format(msg, 1024, message, va_start<2>);
    return DCC_CreatePrivateChannel(user, "OnCreatedPrivateChannel", "s", msg);
}
#else
stock SendPrivateMessage(DCC_User:user, const message[])
{
    return DCC_CreatePrivateChannel(user, "OnCreatedPrivateChannel", "s", message);
}
#endif

stock DCC_Channel:GetChannel(DCC_Message:message)
{
    new DCC_Channel:channel;
    DCC_GetMessageChannel(message, channel);
    return channel;
}
stock DCC_User:GetAuthor(DCC_Message:message)
{
    new DCC_User:author;
    DCC_GetMessageAuthor(message, author);
    return author;
}
stock ReturnUserName(DCC_User:user)
{
    new UserName[DCC_USERNAME_SIZE];
    DCC_GetUserName(user, UserName);
    return UserName;
}
stock bool:IsPrivateChannel()
{
    new DCC_Guild:guild, DCC_Channel:channel, guildId[DCC_ID_SIZE];
    DCC_GetMessageChannel(oldMessage, channel);
    DCC_GetChannelGuild(channel, guild);
    DCC_GetGuildId(guild, guildId);
    if (guildId[0] == '\0') return true;

    return false;
}
stock bool:IsValidMessage(DCC_Message:message)
{
    new msg[DCC_MAX_MESSAGE];
    DCC_GetMessageContent(message, msg);
    if (msg[0] != '\0') return true;

    else return false;
}
stock bool:AllowBotsCMD(bool:boolean)
{
    AllowBotsCommands = boolean;
    return 1;
}
stock SetPrefix(const text[])
{
    if (strlen(text) >= DCMD_PREFIX_LENGHT || text[0] != '\0')
    {
        printf("[DCMD]: SetPrefix('%s') the lenght of the given prefix is invalid.", text);
        return 0;
    }

    format(prefix, DCMD_PREFIX_LENGHT, "%s", text);
    return 1;
}

//Private callbacks.

#if defined FILTERSCRIPT

public OnFilterScriptInit()
{
    dcmd_hasODCR = funcidx("OnDiscordCommandReceived") != -1;
    dcmd_hasODCP = funcidx("OnDiscordCommandPerformed") != -1;

    return CallLocalFunction("dcmd_OnFilterScriptInit", "");
}

#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif
#define OnFilterScriptInit dcmd_OnFilterScriptInit
forward dcmd_OnFilterScriptInit();

#else

public OnGameModeInit()
{
    dcmd_hasODCR = funcidx("OnDiscordCommandReceived") != -1;
    dcmd_hasODCP = funcidx("OnDiscordCommandPerformed") != -1;

    if (funcidx("dcmd_OnGameModeInit") != -1)
    {
        return CallLocalFunction("dcmd_OnGameModeInit", "");
    }
    return 1;
}

#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit dcmd_OnGameModeInit
forward dcmd_OnGameModeInit();

#endif

public OnCreatedPrivateChannel(const message[])
{
    // Do not use this function, this is for send private message to users with SendPrivateMessage.
    new DCC_Channel:channel = DCC_GetCreatedPrivateChannel();
    if (message[0] == '\0') return printf("[DCMD]: Was made an attempt to send an empty message to (%s).", ReturnUserName(GetAuthor(oldMessage)));
    DCC_SendChannelMessage(channel, message);
    return 1;
}

public DCC_OnMessageCreate(DCC_Message:message)
{
    new cmd[512];
    DCC_GetMessageContent(message, cmd);

    if (cmd[0] == '\0') return 0;

    new DCC_User:author;
    DCC_GetMessageAuthor(message, author);

    new bool:isBot;
    DCC_IsUserBot(author, isBot);

    if (isBot && !AllowBotsCommands) return 0;

    new DCC_Channel:channel;
    DCC_GetMessageChannel(message, channel);

    oldMessage = message;

    if (strfind(cmd, prefix, true) != -1)
    {
        new prefixLen = strlen(prefix);
        new fname[68] = "", params[512] = "";
        new index = prefixLen, len = strlen(cmd);


        new cmdIndex = 0;
        while (index < len && cmd[index] != ' ' && cmdIndex < sizeof(fname) - 1)
        {
            fname[cmdIndex++] = cmd[index++];
        }
        fname[cmdIndex] = '\0';

        while (index < len && cmd[index] == ' ') index++;

        format(fname, 68, "dcmd_%s", fname);

        if (index < len)
        {
            strmid(params, cmd, index, len, sizeof(params));
        }

        //printf("CMD:%s(%s)", fname, params);

        if (dcmd_hasODCR && !CallLocalFunction("OnDiscordCommandReceived", "sd", cmd, _:message))
        {
            return 0;
        }

        new result = CallLocalFunction(fname, "ds", _:message, params);
        if (dcmd_hasODCP)
        {
            return CallLocalFunction("OnDiscordCommandPerformed", "sdd", cmd, result, _:message);
        }

        return result;
    }
    return 1;
}

//--------------------------------------------------


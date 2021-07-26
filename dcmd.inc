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
	If you repost this include pls give credits <3
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
#if !defined dcconnector_included
	#error Could not find y_va
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
		forward dcmd_%1(%2); \
		public dcmd_%1(%2)

//--------------------------------------------------
forward OnCreatedPrivateChannel(message[]); //You may not use this callback, use SendPrivateMessage instead.
forward OnDiscordCommandReceived(cmdtext[], DCC_Message:message); //This callback y called first when a command is received.
forward OnDiscordCommandPerformed(cmdtext[], sucess, DCC_Message:message); //This callback is called when a command is evaluated, meanly when the function of cmdtext is called (e.g: dcmd_verifydiscord), if doesn't exist sucess = 1.

new DCC_Message:oldMessage,
	prefix[DCMD_PREFIX_LENGHT] = "!", //You may need to change the initial parameter for prefix, default !
	bool:AllowBotsCommands = false;

static
	bool:dcmd_hasODCR = false,
	bool:dcmd_hasODCP = false;

//--------------------------------------------------

//Utility

stock SendDiscord(DCC_Channel:channel, const text[], {Float, _}:...){
	new msg[512];
	va_format(msg, 512, text, va_start<2>);
	DCC_SendChannelMessage(channel, msg);
	return 1;
}
stock SendPrivateMessage(DCC_User:user, const message[], {Float, _}:...){
	new msg[1024];
	va_format(msg, 1024, message, va_start<2>);
	return DCC_CreatePrivateChannel(user, "OnCreatedPrivateChannel", "s", msg);
}
stock DCC_Channel:GetChannel(DCC_Message:message){
	new DCC_Channel:channel;
	DCC_GetMessageChannel(message, channel);
	return channel;
}
stock DCC_User:GetAuthor(DCC_Message:message){
	new DCC_User:author;
	DCC_GetMessageAuthor(message, author);
	return author;
}
stock ReturnUserName(DCC_User:user){
	new UserName[DCC_USERNAME_SIZE];
	DCC_GetUserName(user, UserName);
	return UserName;
}
stock bool:IsPrivateChannel(){
	new DCC_Guild:guild, DCC_Channel:channel, guildId[DCC_ID_SIZE];
	DCC_GetMessageChannel(oldMessage, channel);
	DCC_GetChannelGuild(channel, guild);
	DCC_GetGuildId(guild, guildId);
	if(isnull(guildId)) return true;
	return false;
}
stock bool:IsValidMessage(DCC_Message:message){
	new msg[DCC_MAX_MESSAGE];
	DCC_GetMessageContent(message, msg);
	if(!isnull(msg)) return true;
	else return false;
}
stock bool:AllowBotsCMD(bool:boolean){ // You can simply do "AllowBotCommands = bool" anyways.
	AllowBotsCommands = boolean;
	return 1;
}
stock SetPrefix(text[]){
	if(strlen(text) >= DCMD_PREFIX_LENGHT || isnull(text)) {print("[DCMD]: SetPrefix('%s') the lenght of the given prefix is invalid.", text); return 0;}
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

public OnCreatedPrivateChannel(message[]){ //Do not use this function, this is for send private message to users with SendPrivateMessage.
	new DCC_Channel:channel = DCC_GetCreatedPrivateChannel();
	if(isnull(message)) return printf("[DCMD]: Was made an attempt to send an empty message to (%s).", ReturnUserName(GetAuthor(oldMessage)));
	DCC_SendChannelMessage(channel, message);
	return 1;
}

public DCC_OnMessageCreate(DCC_Message:message){ // Do not use this function, use OnDiscordCommandReceived and OnDiscordCommandPerformed instead.
	new cmd[512], DCC_Channel:channel, DCC_User:author, bool:isBot, bool:hasPrefix = false;
	DCC_GetMessageAuthor(message, author);
	DCC_IsUserBot(author, isBot);
	DCC_GetMessageChannel(message, channel);
	DCC_GetMessageContent(message, cmd);
	if(isnull(cmd)) return 0;
	if(strfind(cmd, prefix) == 0 && strfind(cmd, prefix) != -1){
		hasPrefix = true;
	}
	if(isBot == true && AllowBotsCommands == false) return 0;
	if(hasPrefix == true){
		oldMessage = message;
		new pos, fname[32];
		while(cmd[++pos] > ' '){
			if(cmd[pos + strlen(prefix)-1] == ' ') continue;
			fname[pos-1] = tolower(cmd[pos + strlen(prefix)-1]);
		}
		if(dcmd_hasODCR && CallLocalFunction("OnDiscordCommandReceived", "sd", fname, _:message) == 0) return 0;
		new params[256];
		format(params, 256, cmd);
		strdel(params, 0, pos+1);
		format(fname, sizeof(fname), "dcmd_%s", fname);
		if(isnull(params)){
			if(dcmd_hasODCP){
				return CallLocalFunction("OnDiscordCommandPerformed", "sid", cmd, CallLocalFunction(fname, "ds", _:message,"\1"), _:message);	
			}
			return CallLocalFunction(fname, "ds", _:message,"\1");
		} else {
			if(dcmd_hasODCP){
				return CallLocalFunction("OnDiscordCommandPerformed", "sid", cmd, CallLocalFunction(fname, "ds", _:message, params), _:message);
			}
			return CallLocalFunction(fname, "ds", _:message, params);
		}
	}
	return 1;
}

//--------------------------------------------------


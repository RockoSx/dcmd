# dcmd
DCMD is a command handler for discord-connector, similar to ZCMD this handler gives you more control in your commands.

Special thanks to: <br>
 maddinat0r - Author of discord-connector https://github.com/maddinat0r/samp-discord-connector<br>
 Y_Less - Author of y_va https://github.com/pawn-lang/YSI-Includes<br>
 Zeex - For the inspiration with zcmd.<br>
 
# Installation
- Prework<br>
Before install DCMD you need to install samp-discord-connector.
https://github.com/maddinat0r/samp-discord-connector
Please, make sure to include your bot token line in server.cfg.<br>
Optional: using the include y_va.inc from Y_less can enable the use of format in functions like
`SendDiscord(DCC_Channel:channel, const text[], {Float, _}:...)` or `SendPrivateMessage(DCC_User:user, const message[], {Float, _}:...)`
https://github.com/pawn-lang/YSI-Includes

- Installation of DCMD<br>
Download the release and copy the file dcmd.inc into your includes folder pawno/includes.
After that you need to add this line in the gamemode/filterscript.
```pawn
#include <dcmd>
```

# Example

For more examples see the file example.pwn

```pawn
DCMD:prefix(DCC_Message:message, params[]){ //With this command you can change the prefix of the commands. Default is "!"
	if(sscanf(params, "s[6]", params[0])) return SendDiscord(GetChannel(message), "Use: %sprefix (Prefix)", prefix);
	else if(strcmp(params[0], prefix) == 0) return SendDiscord(GetChannel(message), ":fleur_de_lis: The prefix already is `%s`!", prefix);
	else if(SetPrefix(params[0]) == 0) return SendDiscord(GetChannel(message), ":negative_squared_cross_mark: Invalid prefix!");
	SendDiscord(GetChannel(message), ":white_check_mark: The prefix has been changed to \"%s\"", prefix);
	return 1;
}
```

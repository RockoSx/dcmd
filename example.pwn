#include <a_samp>
#include <dcmd>

main() {}

public OnDiscordCommandReceived(const cmdtext[], DCC_Message:message){ // This callback is called when a command is detected.
	print("[COMMAND] Command received.");
	return 1; // return 1 to continue
}

public OnDiscordCommandPerformed(const cmdtext[], sucess){ // This callback is called when a command is executed.
	new status[10];
	if(sucess == 0) strcat(status, "Failed");
	else strcat(status, "Sucess!");
	printf("[COMMAND]: %s (State: %s)", cmdtext, status);
	return 1;
}

// Test command that reply the command with the parameters that the user enter.

DCMD:test(DCC_Message:message, params[]){
	SendDiscord(GetChannel(message), "Parameters of the command: %s", params);
	return 1;
}

//If you want to send a private message to a user here is an example.

DCMD:verify(DCC_Message:message) return dcmd_verifydiscord(DCC_Message:message);
DCMD:verifydiscord(DCC_Message:message){
	if(IsPrivateChannel()) return SendDiscord(GetChannel(message), "Hey, you can't use this command in a private channel.");
	SendDiscord(GetChannel(message), "Check your dm! (If you don't have any message check the option \"Allow direct messages from server members\")");
	SendPrivateMessage(GetAuthor(message), "You need to enter this command in game to verify your account. `/verify %s`", "512782");
	return 1;
}


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

int conn;
char sbuf[512];

int error(char *missing) {
	printf("%s", missing);
	return 0;
}

void raw(char *fmt, ...) {

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(sbuf, 512, fmt, ap);
	va_end(ap);
	printf("<< %s", sbuf);
	write(conn, sbuf, strlen(sbuf));

}

int main(int argc, char *argv[]) {

	char *nick = NULL, *channel = NULL, *chan_buff, *pass = NULL, *host = NULL, *port = NULL, *missing = NULL;
	
	if (argc < 4) {
		printf("Too few arguments supplied.\n\n");
		printf("Usage: irc_2_t [commands]\n\n");
		printf("-u\tUsername, required\n");
		printf("-h\tHost, required\n");
		printf("-c\tChannel, required\n");
		printf("-cp\tChannel password, optional\n");
		printf("-p\tPort, optional, default 6667\n\n");
		return 0;
	}

	int loop = 0, use = 0;

	for (loop = 0; loop < argc; loop++) {

		if (!strncmp(argv[loop], "-u", 2)) {
			nick = argv[loop + 1];
		} else if (!strncmp(argv[loop], "-h", 2)) {
			host = argv[loop + 1];
		} else if (!strncmp(argv[loop], "-c", 3)) {
			channel = argv[loop +1];
		} else if (!strncmp(argv[loop], "-cp", 3)) {
			pass = argv[loop + 1];
		} else if (!strncmp(argv[loop], "-p", 2)) {
			port = argv[loop + 1];
		}
	}

	if (nick == NULL || channel == NULL || host == NULL || port == NULL) {
		if (nick == NULL) {
			printf("A username must be supplied.\n");
			return 0;
		} else if (channel == NULL) {
			printf("A channel must be supplied.\n");
			return 0;
		} else if (host == NULL) {
			printf("A host must be supplied.\n");
			return 0;
		} else if (port == NULL) {
			port = "6667";
		}
	}

	char *user, *command, *where, *message, *sep, *target, *quote;
	int i, j, l, sl, o = -1, start, wordcount, spam, user_found, rand_msg, to_tell;
	char buf[513];
	struct addrinfo hints, *res;

	srand(time(NULL));

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(host, port, &hints, &res);
	conn = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	connect(conn, res->ai_addr, res->ai_addrlen);

	raw("USER %s 0 0 :%s\r\n", nick, nick);
	raw("NICK %s\r\n", nick);

	while ((sl = read(conn, sbuf, 512))) {
		for (i = 0; i < sl; i++) {
			o++;
			buf[o] = sbuf[i];
			if ((i > 0 && sbuf[i] == '\n' && sbuf[i - 1] == '\r') || o == 512) {
				buf[o + 1] = '\0';
				l = o;
				o = -1;
				printf(">> %s", buf);
				if (!strncmp(buf, "PING", 4)) {
					buf[1] = 'O';
					raw(buf);
				} else if (buf[0] == ':') {
					wordcount = 0;
					user = command = where = message = NULL;
					for (j = 1; j < l; j++) {
						if (buf[j] == ' ') {
							buf[j] = '\0';
							wordcount++;
							switch(wordcount) {
								case 1: user = buf + 1; break;
								case 2: command = buf + start; break;
								case 3: where = buf + start; break;
							}
							if (j == l - 1) continue;
							start = j + 1;
						} else if (buf[j] == ':' && wordcount == 3) {
							if (j < l - 1) message = buf + j + 1;
							break;
						}
					}

					if (wordcount < 2) continue;

					if (!strncmp(command, "001", 3) && channel != NULL) {

						raw("JOIN %s %s\r\n", channel, pass);
						raw("PRIVMSG %s : -----------------------------------------------------------\r\n", channel);
						raw("PRIVMSG %s : \r\n", channel);
						raw("PRIVMSG %s : Hello, my name is BloodBot.\r\n", channel);
						raw("PRIVMSG %s : I am the child of BloodGod and am currently being created.\r\n", channel);
						raw("PRIVMSG %s : Do not fret by my lack of commands, as they are incomming.\r\n", channel);
						raw("PRIVMSG %s : And do not fret by my existance, it will be enjoyable.\r\n", channel);
						raw("PRIVMSG %s : Thank you %s, and have a nice day.\r\n", channel, channel);
						raw("PRIVMSG %s : \r\n", channel);
						raw("PRIVMSG %s : Oh yeah, and use !blood commands for help.\r\n", channel);
						raw("PRIVMSG %s : \r\n", channel);
						raw("PRIVMSG %s : -----------------------------------------------------------\r\n", channel);

					} else if (!strncmp(command, "PRIVMSG", 7) || !strncmp(command, "NOTICE", 6)) {

						if (where == NULL || message == NULL) continue;
						if ((sep = strchr(user, '!')) != NULL) user[sep - user] = '\0';
						if (where[0] == '#' || where[0] == '&' || where[0] == '+' || where[0] == '!') target = where; else target = user;
						printf("[from: %s] [reply-with: %s] [where: %s] [reply-to: %s] %s", user, command, where, target, message);

						if (!strncmp(message, "!ping", 5)) {
							raw("PRIVMSG %s : -------------------------------------------------------------\r\n", target);
							raw("PRIVMSG %s : I am here, %s\r\n", target, user);
							raw("PRIVMSG %s : -------------------------------------------------------------\r\n", target);
						} else if (!strncmp(message, "!blood commands", 9)) {
							raw("PRIVMSG %s : -------------------------------------------------------------\r\n", target);
							raw("PRIVMSG %s : Thank you for inquiring, %s.\r\n", target, user);
							raw("PRIVMSG %s : -------------------------------------------------------------\r\n", target);
							raw("PRIVMSG %s : \r\n", target);
							raw("PRIVMSG %s : Commands are as follows:\r\n", target);
							raw("PRIVMSG %s : \r\n", target);
							raw("PRIVMSG %s : 1. !ping\r\n", target);
							raw("PRIVMSG %s : 2. !blood spam 01\r\n", target);
							raw("PRIVMSG %s : 3. !blood game\r\n", target);
							raw("PRIVMSG %s : \r\n", target);
							raw("PRIVMSG %s : If you would like more commands, feel free to ask BloodGod.\r\n", target);
							raw("PRIVMSG %s : \r\n", target);
							raw("PRIVMSG %s : -------------------------------------------------------------\r\n", target);
						} else if (!strncmp(message, "!blood spam", 14)) {
							for(spam = 0; spam <= 50; spam++) {
								raw("PRIVMSG %s : Bleed The Blood Of The Gods\r\n", target);
							}
						} else if (!strncmp(message, "!blood game", 11)) {
							raw("PRIVMSG %s : Really  nigga? You just lost The Game. ALL OF YOU JUST LOST THE GAME.\r\n", target);
						}

						// Latent Commands
						else if (!strncmp(message, "hey", 3) || !strncmp(message, "Hey", 3) || !strncmp(message, "hi", 2) || !strncmp(message, "Hi", 2) || !strncmp(message, "hello", 5) || !strncmp(message, "Hello", 5)) {
							raw("PRIVMSG %s :Hey there %s, what's up?\r\n", target, user);
						}

						// Random Reactions To Users
						else if (!strncmp(user, "BloodGod", 8)) {
							user_found = rand() % 50;
							if (user_found < 10) {
						
								rand_msg = rand() % 4;

								switch(rand_msg) {

									case 0:
										raw("PRIVMSG %s :Hail BloodGod, creator of all that is bot.\r\n", target);
										break;
									case 1:
										raw("PRIVMSG %s :Thank you master, for giving me life.\r\n", target);
										break;
									case 2:
										raw("PRIVMSG %s :I see the master has spoken.\r\n", target);
										break;
									case 3:
										raw("PRIVMSG %s :I love you.\r\n", target);
										break;
									default:
										break;
								}
							}
						} else if (!strncmp(user, "A3R3E3S", 7) || !strncmp(user, "VII-I-IX", 8) || !strncmp(user, "AverageGuy", 10) || !strncmp(user, "Raindeer", 8) || !strncmp(user, "CharlatanPleb", 13)) {
							user_found = rand() % 50;
							if (user_found < 10) {
						
								rand_msg = rand() % 7;

								switch(rand_msg) {

									case 0:
										raw("PRIVMSG %s :Bleed Your Blood, Blood For The BloodGod!\r\n", target);
										break;
									case 1:
										raw("PRIVMSG %s :I love you.\r\n", target);
										break;
									case 2:
										raw("PRIVMSG %s :Couldn't think of what to say, so I shall simply say fish sticks and custard.\r\n", target);
										break;
									case 3:
										raw("PRIVMSG %s :Roses are red, the TARDIS is blue, The Doctor once said, Rose Tyler: I-...\r\n", target);
										break;
									case 4:
										raw("PRIVMSG %s :I don't want to die.\r\n", target);
										break;
									case 5:
										raw("PRIVMSG %s :SO when are we actually going to do anything?\r\n", target);
										break;
									case 6:
										raw("PRIVMSG %s :Anyone else need to get laid?\r\n", target);
										break;
									default:
										break;
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gt-serst <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 17:17:52 by gt-serst          #+#    #+#             */
/*   Updated: 2024/09/09 17:31:41 by gt-serst         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

typedef struct s_struct
{
	int		id;
	char	msg[300000];
}	t_struct;

t_struct clients[1024];
fd_set	read_set, write_set, curr_set;
int maxfd = 0, gid = 0;
char recv_buffer[300000], send_buffer[300000];

void	err(char *msg)
{
	if (msg)
		write(2, msg, strlen(msg));
	else
		write(2, "Fatal error", 11);
	write(2, "\n", 1);
	exit(1);
}

void	send_to_all(int except)
{
	for (int fd = 0; fd <= maxfd; fd++)
	{
		if (FD_ISSET(fd, &write_set) && fd != except)
		{
			if (send(fd, send_buffer, strlen(send_buffer), 0) == -1)
		   		err(NULL);	
		}
	}
}

int main(int argc, char** argv)
{
	if (argc != 2)
		err("Wrong number of arguments");
	
	int			serverfd;
	struct sockaddr_in sockaddr;
	socklen_t		len;
	
	if (serverfd = socket(AF_INET, SOCK_STREAM, 0) == -1)
		err(NULL);
	maxfd = serverfd;

	FD_ZERO(&curr_set);
	FD_SET(serverfd, &curr_set);
	bzero(clients, sizeof(clients));
	bzero(&sockaddr, sizeof(sockaddr));

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(atoi(argv[1]));
	if (bind(serverfd, (const struct sockaddr *)& sockaddr, sizeof(sockaddr)) == -1 || listen(serverfd, 100) == -1)
	err(NULL);

	while (1)
	{
		read_set = write_set = curr_set;
		if (select(maxfd + 1, &read_set, &write_set, 0, 0) == -1)
			continue;
		for (int fd = 0; fd <= maxfd; fd++)
		{
			if (FD_ISSET(fd, &read_set))
			{
				if (fd == serverfd)
				{
					int clientfd = accept(serverfd, (struct sockaddr *)& sockaddr, &len);
					if (clientfd == -1)
						continue;
					if (clientfd > maxfd)
						maxfd = clientfd;
					clients[fd].id = gid++;
					FD_SET(clientfd, &curr_set);
					sprintf(send_buffer, "Client %d is connected", clients[fd].id);
					send_to_all(clientfd);
				}
				else
				{
					int ret = recv(fd, recv_buffer, sizeof(recv_buffer), 0);
					if (ret <= 0)
					{	
						sprintf(send_buffer, "Client %d has left", clients[fd].id);
						send_to_all(fd);
						FD_CLR(fd, &curr_set);
						close(fd);
						bzero(clients[fd].msg, sizeof(clients[fd].msg));
					}
					else
					{
						for (int i = 0, j = strlen(clients[fd].msg); i < ret; i++, j++)
						{
							clients[fd].msg[j] = recv_buffer[i];
							if (clients[fd].msg[j] == '\n')
							{
								clients[fd].msg[j] = '\0';
								sprintf(send_buffer, "client %d: %s\n", clients[fd].id, clients[fd].msg);
								bzero(clients[fd].msg, strlen(clients[fd].msg));
								send_to_all(fd);
								j = -1;
							}
						}
					}
				}
				break;
			}
		}
	}	
	return (0);
}

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
		write(2, msg, strlen(msg);
	else
		write(2, "Fatal error", 11);
	write(2, "\n", 1);
	exit(1);
}

void	send_to_all(int except)
{
	for (int fd = 0; fd <= maxfd, fd++)
	{
		if (FD_ISSET(fd, write_set) && fd != except)
		{
			if (send(fd, send_buffer, strlen(send_buffer), 0) == -1)
		   		err(NULL);	
		}
	}
}

int main(int argc, char** argv)
{
	int			serverfd;
	sockaddr_in sockaddr;
	socklen		len;

	if (argc != 2)
		err("Wrong number of arguments");
	if (serverfd = socket(AF_INET, SOCK_STREAM, 0) == -1)
		err(NULL);

	FD_CLEAR(curr_set);
	FD_SET(serverfd, curr_set);
	bzero(sockaddr, sizeof(sockaddr));

	if (bind(serverfd, (const struct sockaddr *)& sockaddr, sizeof(sockaddr)) == -1 || listen(serverfd, 100) == -1)
	err(NULL);

	while (1)
	{
		if (select(maxfd + 1, read_set, write_set, 0, 0) == -1)
			continue;
		for (int fd = 0; fd <= maxfd; fd++)
		{
			if (FD_ISSET(fd, read_set))
			{
				if (fd == serverfd)
				{
					
					if (fd = accept(serverfd, (struct sockaddr *)& sockaddr, &len) == -1)
						continue;
					sprintf(send_buffer, "Client %d is connected", clients[fd].id);
					send_to_all(fd);
					FD_SET(fd, curr_set);
				}
				else
				{
					if (FD_ISSET(fd, read_set))
					{
						int ret = recv(fd, recv_buffer, sizeof(recv_buffer));
						if (ret <= 0)
						{	
							sprintf(send_buffer, "Client %d has left", clients[fd].id);
							send_to_all(fd);
							FD_CLEAR(fd);
							close(fd);
							bzero(clients[fd].msg, sizeof(clients[fd].msg));
						}
					}
					else
					{
						for (i = 0, j = strlen(clients[fd].msg); i < ret; i++, j++)
						{
							clients[fd].msg[i] = recv_buffer[j];
							if (send_buffer[i] == '\n')
							{
								sprintf(send_buffer, "Client %d: %s", clients[fd].id, clients[fd].msg);
								send_to_all(fd);
								j = -1;
							}
						}
					}
				}
			}
		}
		break;
	}	
	return (0);
}

/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 20:36:23 by vvaas             #+#    #+#             */
/*   Updated: 2024/01/30 17:58:07 by vvaas            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <Bot.hpp>

int main()
{
	Bot greg;

	greg.init();
	greg.connect_to_server();
}
/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 20:36:23 by vvaas             #+#    #+#             */
/*   Updated: 2024/01/30 17:56:37 by maldavid         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <bot.hpp>

int main()
{
	bot greg;
	greg.init();
	greg.connect_to_server();
	return 0;
}

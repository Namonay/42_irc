/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 09:27:04 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/20 19:19:39 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unstd/unique_ptr.hpp>
#include <unstd/shared_ptr.hpp>
#include <unstd/weak_ptr.hpp>
#include <ansi.hpp>

int main(int ac, char** av)
{
	(void)ac;
	(void)av;
	std::cout << AnsiColor::red << "this" << AnsiColor::blue << " is" << AnsiColor::green << " a" << AnsiColor::yellow << " test" << AnsiColor::reset << std::endl;
	return 0;
}

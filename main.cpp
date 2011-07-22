#include <fcppt/container/array.hpp>
#include <fcppt/container/bitfield/basic.hpp>

typedef unsigned field_value;

typedef fcppt::container::array<
	field_value,
	5
> field_row;

typedef fcppt::container::array<
	field_row,
	6
> field;

namespace direction
{
enum type
{
	west,
	north,
	east,
	south,
	size
};
}

typedef fcppt::container::bitfield::basic<
	direction::type,
	direction::size
> field_state;

typedef fcppt::container::array<
	field_state,
	5
> field_state_row;

typedef fcppt::container::array<
	field_state_row,
	6
> state_field;

bool
calculate_step(
	field,
	field::size_type _x,
	field::size_type _y,
	unsigned _steps
);

#include <iostream>
#include <ostream>

bool
loop(
	field const &_field,
	unsigned const _steps
)
{
	field::size_type count(0);

	for(
		field::size_type y(0);
		y < _field.size();
		++y
	)
		for(
			field::size_type x(0);
			x < _field[y].size();
			++x
		)
			count += _field[y][x];

//	std::cout << "count: " <<  count << '\n';

	if(
		count == 0
	)
		return true;

//	std::cout << "steps: " <<  _steps << '\n';

	if(
		_steps == 0
	)
		return false;

	for(
		field::size_type y(0);
		y < _field.size();
		++y
	)
		for(
			field::size_type x(0);
			x < _field[y].size();
			++x
		)
			if(
				calculate_step(
					_field,
					x,
					y,
					_steps - 1
				)
			)
				return true;

	return false;
}

unsigned
count_bits(
	field_state const &_state
)
{
	unsigned ret(0);

	for(
		field_state::const_iterator it(
			_state.begin()
		);
		it != _state.end();
		++it
	)
		if(*it)
			++ret;
	
	return ret;
}

field const
calculate_explosion(
	field _field,
	field::size_type const _x,
	field::size_type const _y
)
{
//	std::cout << "calculate explosion " << _x << ' ' << _y << '\n';

	state_field states;

	for(
		state_field::size_type y(0);
		y < states.size();
		++y
	)
		for(
			state_field::size_type x(0);
			x < states[y].size();
			++x
		)
			states[y][x] = field_state::null();
	
	states[_y][_x] =
		field_state(direction::west)
		| direction::north
		| direction::east
		| direction::south;
	
	_field[_y][_x] = 0;

	state_field new_states(
		states
	);

	for(
		;;
	)
	{
		for(
			state_field::size_type y(0);
			y < states.size();
			++y
		)
			for(
				state_field::size_type x(0);
				x < states[y].size();
				++x
			)
			{
				field_state const state(
					states[y][x]
				);

				if(
					!state
				)
					continue;

				if(
					state & direction::west
					&& x > 0
				)
				{
					new_states[y][x-1] |= direction::west;

//					std::cout << "to west: " << y << ' ' <<  (x-1) << '\n';

					new_states[y][x].set(
						direction::west,
						false
					);
				}

				if(
					state & direction::north
					&& y > 0
				)
				{
					new_states[y-1][x] |= direction::north;

//					std::cout << "to north: " << (y-1) << ' ' <<  x << '\n';

					new_states[y][x].set(
						direction::north,
						false
					);
				}

				if(
					state & direction::east
					&& x < states[y].size() - 1
				)
				{
					new_states[y][x+1] |= direction::east;

//					std::cout << "to east: " << y << ' ' <<  (x+1) << '\n';

					new_states[y][x].set(
						direction::east,
						false
					);
				}

				if(
					state & direction::south
					&& y < states.size() - 1
				)
				{
					new_states[y+1][x] |= direction::south;

//					std::cout << "to south: " << (y+1) << ' ' <<  x << '\n';

					new_states[y][x].set(
						direction::south,
						false
					);
				}
			}
	
		for(
			state_field::size_type y(0);
			y < new_states.size();
			++y
		)
			for(
				state_field::size_type x(0);
				x < new_states[y].size();
				++x
			)
			{
				field_state const cur_state(
					new_states[y][x]
				);

				if(
					cur_state
					&& _field[y][x] != 0
				)
				{
					//std::cout << "hit" << y << ' ' << x << '\n';

					state_field::size_type const elements(
						count_bits(
							cur_state
						)
					);

					new_states[y][x].clear();

					if(
						_field[y][x]
						<= elements
					)
						_field[y][x] = 0;
					else
						_field[y][x] -= elements;

					if(
						_field[y][x] == 0
					)
						new_states[y][x] =
							field_state(direction::west)
							| direction::north
							| direction::east
							| direction::south;
				}
			}


		if(
			states == new_states
		)
			break;

		states = new_states;
	}

	return _field;
}

bool
calculate_step(
	field _field,
	field::size_type const _x,
	field::size_type const _y,
	unsigned const _steps
)
{
	bool ret = false;

	switch(
		_field[_y][_x]
	)
	{
	case 0:
		return false;;
	case 1:
		ret =
			loop(
				calculate_explosion(
					_field,
					_x,
					_y
				),
				_steps
			);
		break;
	case 2:
	case 3:
	case 4:
		--_field[_y][_x];

		ret =
			loop(
				_field,
				_steps
			);
	}

	if(
		ret
	)
		std::cout << _x << ' ' << _y << '\n';
	
	return ret;
}

#include <iostream>
#include <ostream>

int
main()
{
	field const myfield =
	{{
		{1, 0, 0, 4, 3},
		{4, 1, 0, 1, 3},
		{3, 2, 2, 4, 0},
		{2, 1, 0, 1, 3},
		{0, 3, 2, 1, 1},
		{1, 4, 0, 4, 4}
	}};

	unsigned const steps(
		2
	);

	std::cout
		<<
		std::boolalpha
		<<
		loop(
			myfield,
			steps
		)
		<< '\n';
}

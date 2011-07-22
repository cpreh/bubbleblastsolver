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

void
calculate_step(
	field,
	field::size_type _x,
	field::size_type _y,
	unsigned _steps
);

void
loop(
	field const &_field,
	unsigned const _steps
)
{
	if(
		_steps == 0
	)
		return;

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
			calculate_step(
				_field,
				x,
				y,
				_steps - 1
			);
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

	bool done = true;

	while(
		done
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
					states[y][x-1] |= direction::west;

					states[y][x].set(
						direction::west,
						false
					);
				}

				if(
					state & direction::north
					&& y > 0
				)
				{
					states[y-1][x] |= direction::north;

					states[y][x].set(
						direction::north,
						false
					);
				}

				if(
					state & direction::east
					&& x < states[y].size() - 1
				)
				{
					states[y][x+1] |= direction::east;

					states[y][x].set(
						direction::east,
						false
					);
				}

				if(
					state & direction::south
					&& y < states.size() - 1
				)
				{
					states[y+1][x] |= direction::south;

					states[y][x].set(
						direction::south,
						false
					);
				}
			}
	
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
				field_state const cur_state(
					states[y][x]
				);

				if(
					cur_state
					&& _field[y][x] != 0
				)
				{
					switch(
						_field[y][x]
					)
					{
					case 0:
						break;
					case 1:
						_field[y][x] = 0;

						states[y][x] =
							field_state(direction::west)
							| direction::north
							| direction::east
							| direction::south;
						break;
					case 2:
					case 3:
					case 4:
						{
							state_field::size_type const elements(
								count_bits(
									cur_state
								)
							);

							if(
								_field[y][x]
								<= elements
							)
								_field[y][x] = 0;
							else
								_field[y][x] -= elements;

							if(
								_field[y][x] <= 1
							)
								states[y][x] =
									field_state(direction::west)
									| direction::north
									| direction::east
									| direction::south;
						}
					}
				}
			}

		done = true;

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
				if(
					states[y][x]
				)
				{
					done = false;
					break;
				}
			}

	}

	return _field;
}

void
calculate_step(
	field _field,
	field::size_type const _x,
	field::size_type const _y,
	unsigned const _steps
)
{
	switch(
		_field[_y][_x]
	)
	{
	case 0:
		return;
	case 1:
		loop(
			calculate_explosion(
				_field,
				_x,
				_y
			),
			_steps
		);
	case 2:
	case 3:
	case 4:
		--_field[_y][_x];

		loop(
			_field,
			_steps
		);
	}
}

int
main()
{
	field const myfield =
	{
	};

	unsigned const steps(
		6
	);

	loop(
		myfield,
		steps
	);
}

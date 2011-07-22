#include <fcppt/container/array.hpp>
#include <fcppt/container/bitfield/basic.hpp>
#include <algorithm>
#include <iostream>
#include <ostream>

namespace
{

typedef unsigned field_value;

template<
	typename T
>
struct make_array
{
	typedef fcppt::container::array<
		fcppt::container::array<
			T,
			5
		>,
		6
	> type;
};

typedef make_array<
	field_value
>::type field;

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

typedef make_array<
	field_state
>::type state_field;

#define ITERATE_ARRAY(\
	array,\
	p_x,\
	p_y\
)\
	for(\
		field::size_type p_y(0);\
		p_y < _field.size();\
		++p_y\
	)\
		for(\
			field::size_type p_x(0);\
			p_x < _field[p_y].size();\
			++p_x\
		)

bool
calculate_step(
	field,
	field::size_type _x,
	field::size_type _y,
	unsigned _steps
);

bool
loop(
	field const &_field,
	unsigned const _steps
)
{
	{
		field::size_type count(0);

		ITERATE_ARRAY(
			_field,
			x,
			y
		)
			count += _field[y][x];

		if(
			count == 0
		)
			return true;
	}

	if(
		_steps == 0
	)
		return false;

	ITERATE_ARRAY(
		_field,
		x,
		y
	)
		if(
			::calculate_step(
				_field,
				x,
				y,
				_steps - 1
			)
		)
			return true;

	return false;
}

inline
void
spawn_bullet(
	field_state &_state
)
{
	_state =
		field_state(direction::west)
		| direction::north
		| direction::east
		| direction::south;
}

inline
state_field::size_type
direction_x(
	state_field::size_type const _pos,
	direction::type const _direction
)
{
	switch(
		_direction
	)
	{
	case direction::west:
		return _pos - 1;
	case direction::east:
		return _pos + 1;
	default:
		return _pos;
	}
}

inline
state_field::size_type
direction_y(
	state_field::size_type const _pos,
	direction::type const _direction
)
{
	switch(
		_direction
	)
	{
	case direction::north:
		return _pos - 1;
	case direction::south:
		return _pos + 1;
	default:
		return _pos;
	}
}

inline
void
move_bullet(
	state_field &_states,
	state_field::size_type const _x,
	state_field::size_type const _y,
	direction::type const _direction
)
{
	_states[
		::direction_y(
			_y,
			_direction
		)
	][
		::direction_x(
			_x,
			_direction
		)
	]
	|= _direction;

	_states[_y][_x].set(
		_direction,
		false
	);
}

field const
calculate_explosion(
	field _field,
	field::size_type const _x,
	field::size_type const _y
)
{
	state_field states;

	ITERATE_ARRAY(
		states,
		x,
		y
	)
		states[y][x] = field_state::null();
	
	::spawn_bullet(
		states[_y][_x]
	);

	_field[_y][_x] = 0;

	state_field new_states(
		states
	);

	for(
		;;
	)
	{
		ITERATE_ARRAY(
			states,
			x,
			y
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
				::move_bullet(
					new_states,
					x,
					y,
					direction::west
				);

			if(
				state & direction::north
				&& y > 0
			)
				::move_bullet(
					new_states,
					x,
					y,
					direction::north
				);

			if(
				state & direction::east
				&& x < states[y].size() - 1
			)
				::move_bullet(
					new_states,
					x,
					y,
					direction::east
				);

			if(
				state & direction::south
				&& y < states.size() - 1
			)
				::move_bullet(
					new_states,
					x,
					y,
					direction::south
				);
		}
	
		ITERATE_ARRAY(
			new_states,
			x,
			y
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
				field_value const elements(
					static_cast<
						field_value
					>(
						std::count(
							cur_state.begin(),
							cur_state.end(),
							true
						)
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
					_field[y][x] == 0
				)
					::spawn_bullet(
						new_states[y][x]
					);
				else
					new_states[y][x].clear();
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
			::loop(
				::calculate_explosion(
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
			::loop(
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

}

int
main()
{
	field const myfield =
	{{
		{{1, 0, 0, 4, 3}},
		{{4, 1, 0, 1, 3}},
		{{3, 2, 2, 4, 0}},
		{{2, 1, 0, 1, 3}},
		{{0, 3, 2, 1, 1}},
		{{1, 4, 0, 4, 4}}
	}};

	unsigned const steps(
		2
	);

	std::cout
		<<
		std::boolalpha
		<<
		::loop(
			myfield,
			steps
		)
		<< '\n';
}

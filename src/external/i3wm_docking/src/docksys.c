#include "docksys.h"
#include "data.h"
#include "con.h"
#include "tree.h"
#include "move.h"
#include "log.h"
#include "output.h"
#include "render.h"
#include "workspace.h"
#include <stddef.h>

DockSysCallbacks* g_callbacks = 0;

static int prev_mouse_x;
static int prev_mouse_y;

int handle_button_press(void* user_data, int x, int y, int mxd, int myd, bool lmb_down);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void docksys_set_callbacks(DockSysCallbacks* callbacks)
{
	g_callbacks = callbacks;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void docksys_horizontal_split(Con *con, void *user_data);
void docksys_verical_split(Con *con, void *user_data);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Con* docksys_create_workspace(const char *name)
{
	return workspace_get("test_ws", NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void docksys_create(int x, int y, int width, int height)
{
	tree_init((I3Rect) { 0, 0, (uint32_t)width, (uint32_t)height });
	fake_outputs_init(0, 0, (uint32_t)width, (uint32_t)height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void docksys_set_mouse(void *user_data, int x, int y, bool leftDown)
{
	int mxd = x - prev_mouse_x;
	int myd = y - prev_mouse_y;

	handle_button_press(user_data, x, y, mxd, myd, leftDown);

	prev_mouse_x = x;
	prev_mouse_y = y;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void docksys_update()
{
	tree_render();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Con* docksys_con_by_user_data(void* user_data)
{
	return con_by_user_data(user_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void split(Con *con, void* user_data, orientation_t orientation)
{
	if (con)
	{
		tree_split(con, orientation);
		tree_open_con(con->parent, user_data);
	}
	else
	{
		tree_open_con(NULL, user_data);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void docksys_horizontal_split(Con *con, void *user_data)
{
	split(con, user_data, HORIZ);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void docksys_vertical_split(Con *con, void *user_data)
{
	split(con, user_data, VERT);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void docksys_close_con(void* user_data)
{
	con_focus(con_by_user_data(user_data));
	tree_close_con(DONT_KILL_WINDOW);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void docksys_update_size(int width, int height)
{
	(void)width;
	(void)height;

}




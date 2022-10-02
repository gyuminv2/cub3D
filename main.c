# include <fcntl.h> // open
# include <unistd.h> // close, read, write, 
# include <stdio.h> // printf, perror
# include <stdlib.h> // malloc, free, exit
# include <string.h> // strerror
# include <math.h>
# include "./minilibx_opengl_20191021/mlx.h"

/**
 * @brief WASD
 * 
 */
# define W 13
# define A 0
# define S 1
# define D 2

/**
 * @brief ESC
 * 
 */
# define ESC 53

/**
 * @brief RESOLUTION, MAP_SIZE
 * 
 */
# define WIN_WIDTH 640
# define WIN_HEIGHT 480
# define map_Width	5
# define map_Height	5

/**
 * @brief TOOL
 * 
 */
# define ERROR "ERROR\n"
# define GOOD_END 0
# define BAD_END 1

int map[map_Width][map_Height] = {
		{1, 1, 0, 1, 1},
		{1, 0, 0, 0, 1},
		{1, 0, 0, 0, 1},
		{1, 0, 0, 0, 1},
		{1, 1, 1, 1, 1}
	};

typedef struct s_mlx
{
	void	*ptr;
	void	*win;
	void	*img;
	int		*addr;

	int		w;
	int		h;

	int		pixel;
	int		size;
	int		endian;
}				t_mlx;

typedef struct s_vector
{
	double p_posX;
	double p_posY;
	double p_dirX;
	double p_dirY;
	double planeX;
	double planeY;
	double cameraX;
	double rayDirectionX;
	double rayDirectionY;
	double p_Speed;
	double rotSpeed;
}				t_vector;

typedef struct s_dda
{
	double sideDistX;
	double sideDistY;
	double deltaDistX;
	double deltaDistY;
	double perpWallDist;
	int stepX;
	int stepY;
}				t_dda;

typedef struct s_game
{
	t_mlx	mlx;
	t_vector vector;
	int		buf[480][640];
	int		*wall;
}				t_game;

int ft_strlen(char *str)
{
	int i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	game_init(t_game *game)
{
	game->vector.p_posX = 3.5;
	game->vector.p_posY = 3.5;
	game->vector.p_dirX = -1.0;
	game->vector.p_dirY = 0.0;
	game->vector.planeX = 0.0;
	game->vector.planeY = 0.66;
	game->vector.p_Speed = 0.1;
	game->vector.rotSpeed = 0.1;
}

int	game_loop(t_game *game)
{
	t_dda dda;
	int x;

	x = 0;
	while (x < WIN_WIDTH)
	{
		game->vector.cameraX = (2 * x / (double)(WIN_WIDTH)) - 1;
		
		game->vector.rayDirectionX = game->vector.p_dirX + game->vector.planeX * game->vector.cameraX;
		game->vector.rayDirectionY = game->vector.p_dirY + game->vector.planeY * game->vector.cameraX;

		int mapX = (int)(game->vector.p_posX);
		int mapY = (int)(game->vector.p_posY);

		dda.deltaDistX = fabs(1 / game->vector.rayDirectionX);
		dda.deltaDistY = fabs(1 / game->vector.rayDirectionY);

		int hit = 0;
		int side;

		if (game->vector.rayDirectionX < 0)
		{
			dda.stepX = -1;
			dda.sideDistX = (game->vector.p_posX - mapX) * dda.deltaDistX;
		}
		else
		{
			dda.stepX = 1;
			dda.sideDistX = (mapX + 1.0 - game->vector.p_posX) * dda.deltaDistX;
		}
		if (game->vector.rayDirectionY < 0)
		{
			dda.stepY = -1;
			dda.sideDistY = (game->vector.p_posY - mapY) * dda.deltaDistY;
		}
		else
		{
			dda.stepY = 1;
			dda.sideDistY = (mapY + 1.0 - game->vector.p_posY) * dda.deltaDistY;
		}

		while (hit == 0)
		{
			if (dda.sideDistX < dda.sideDistY)
			{
				dda.sideDistX += dda.deltaDistX;
				mapX += dda.stepX;
				side = 0;
			}
			else
			{
				dda.sideDistY += dda.deltaDistY;
				mapY += dda.stepY;
				side = 1;
			}
			if (map[mapX][mapY] > 0)
				hit = 1;
		}
		if (side == 0)
			dda.perpWallDist = (mapX - game->vector.p_posX + (1 - dda.stepX) / 2) / game->vector.rayDirectionX;
		else
			dda.perpWallDist = (mapY - game->vector.p_posY + (1 - dda.stepY) / 2) / game->vector.rayDirectionY;
		int lineHeight = (int)(WIN_HEIGHT / dda.perpWallDist);
		int drawStart = (-lineHeight / 2) + (WIN_HEIGHT / 2);
		if (drawStart < 0)
			drawStart = 0;
		int drawEnd = (lineHeight / 2) + (WIN_HEIGHT / 2);
		if (drawEnd >= WIN_HEIGHT)
			drawEnd = WIN_HEIGHT - 1;

		double wallX;
		if (side == 0)
			wallX = game->vector.p_posY + dda.perpWallDist * game->vector.rayDirectionY;
		else
			wallX = game->vector.p_posX + dda.perpWallDist * game->vector.rayDirectionX;
		wallX -= floor(wallX);

		int texX = (int)(wallX * (double)64);
		if (side == 0 && game->vector.rayDirectionX > 0)
			texX = 64 - texX - 1;
		if (side == 1 && game->vector.rayDirectionY < 0)
			texX = 64 - texX - 1;
		double step = 1.0 * 64 / lineHeight;
		double texPos = (drawStart - WIN_HEIGHT / 2 + lineHeight / 2) * step;
		for (int y = drawStart; y < drawEnd; y++)
		{
			int texY = (int)texPos & (64 - 1);
			texPos += step;
			int color = game->wall[64 * texY + texX];
			if (side == 1)
				color = (color >> 1) & 8355711;
			game->buf[y][x] = color;
		}
		x++;
	}

	 for (int y = 0; y < 480; y++)
        for (int x = 0; x < 640; x++)
            game->mlx.addr[y * 640 + x] = game->buf[y][x];

	mlx_put_image_to_window(game->mlx.ptr, game->mlx.win, game->mlx.img, 0, 0);
	return 0;
}

// int input_key(int key, t_game *game)
// {

// }

void	show_wall(t_game *game)
{
	t_mlx	img;

	img.img = mlx_xpm_file_to_image(game->mlx.ptr, "wall_w.xpm", &img.w, &img.h);
	img.addr = (int *)mlx_get_data_addr(img.img, &img.pixel, &img.size, &img.endian);
	int y = 0;
	while (y < img.h)
	{
		int x = 0;
		while (x < img.w)
		{
			game->wall[x + img.w * y] = img.addr[x + img.w * y];
			x++;
		}
		y++;
	}
	mlx_destroy_image(game->mlx.ptr, img.img);
}

int main()
{
	t_game	*game;

	if (!(game = malloc(sizeof(t_game))))
	{
		write(1, ERROR, ft_strlen(ERROR));
		return BAD_END;
	}
	if (!(game->wall = (int *)malloc(sizeof(int) * 64 * 64)))
	{
		write(1, ERROR, ft_strlen(ERROR));
		return BAD_END;
	}

	game_init(game);
	
	game->mlx.ptr = mlx_init();

	show_wall(game);

	game->mlx.win = mlx_new_window(game->mlx.ptr, WIN_WIDTH, WIN_HEIGHT, "cub3D");
	game->mlx.img = mlx_new_image(game->mlx.ptr, WIN_WIDTH, WIN_HEIGHT);
	game->mlx.addr = (int *)mlx_get_data_addr(game->mlx.img, &game->mlx.pixel, &game->mlx.size, &game->mlx.endian);
	mlx_loop_hook(game->mlx.ptr , &game_loop, game);
    // mlx_hook(game->mlx.win, 2, 0, &input_key, &game);
	mlx_loop(game->mlx.ptr);

	free(game);
	return GOOD_END;
}

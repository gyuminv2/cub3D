# include <fcntl.h> // open
# include <unistd.h> // close, read, write, 
# include <stdio.h> // printf, perror
# include <stdlib.h> // malloc, free, exit
# include <string.h> // strerror
# include <math.h>
# include "./minilibx_opengl_20191021/mlx.h"

/* WASD */
# define W 13
# define A 0
# define S 1
# define D 2

/* ESC */
# define ESC 53

# define ERROR "ERROR\n"
# define GOOD_END 0
# define BAD_END 1
# define WIN_WIDTH 640
# define WIN_HEIGHT 480

int map[5][5] = {
		{1, 1, 1, 1, 1},
		{1, 0, 0, 0, 1},
		{1, 0, 'N', 0, 1},
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
	double playerSpeed;
	double rotSpeed;
}				t_vector;

typedef struct s_game
{
	t_mlx	mlx;
	t_vector vector;
	int		buf[480][640];
	int		*wall;
}				t_game;

void	*mlx_new_window(void *mlx_ptr, int size_x, int size_y, char *title);
void	*mlx_new_image(void *mlx_ptr, int width, int height);
char	*mlx_get_data_addr(void *img_ptr, int *bits_per_pixel, int *size_line, int *endian);
int		mlx_put_image_to_window(void *mlx_ptr, void *win_ptr, void *img_ptr, int x, int y);

int ft_strlen(char *str)
{
	int i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	print_map(int map[5][5])
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			printf("%d ", map[i][j]);
		}
		printf("\n");
	}
}

void	game_init(t_game *game)
{
	game->vector.p_posX = 22.0;
	game->vector.p_posY = 11.5;
	game->vector.p_dirX = -1.0;
	game->vector.p_dirY = 0.0;
	game->vector.planeX = 0.0;
	game->vector.planeY = 0.66;
	game->vector.playerSpeed = 0.1;
	game->vector.rotSpeed = 0.1;
}

int	game_loop(t_game *game)
{
	int x;

	x = 0;
	while (x < WIN_WIDTH)
	{
		double cameraX = (2 * x / (double)(WIN_WIDTH)) - 1;
		
		double rayDirectionX = game->vector.p_dirX + game->vector.planeX * cameraX;
		double rayDirectionY = game->vector.p_dirY + game->vector.planeY * cameraX;

		int mapX = (int)(game->vector.p_posX);
		int mapY = (int)(game->vector.p_posY);

		double sideDistX;
		double sideDistY;

		double deltaDistX = fabs(1 / rayDirectionX);
		double deltaDistY = fabs(1 / rayDirectionY);

		double perpWallDist;

		int stepX;
		int stepY;

		int hit = 0;
		int side;

		if (rayDirectionX < 0)
		{
			stepX = -1;
			sideDistX = (game->vector.p_posX - mapX) * deltaDistX;
		}
		else
		{
			stepX = 1;
			sideDistX = (mapX + 1.0 - game->vector.p_posX) * deltaDistX;
		}
		if (rayDirectionY < 0)
		{
			stepY = -1;
			sideDistY = (game->vector.p_posY - mapY) * deltaDistY;
		}
		else
		{
			stepY = 1;
			sideDistY = (mapY + 1.0 - game->vector.p_posY) * deltaDistY;
		}

		while (hit == 0)
		{
			if (sideDistX < sideDistY)
			{
				sideDistX += deltaDistX;
				mapX += stepX;
				side = 0;
			}
			else
			{
				sideDistY += deltaDistY;
				mapY += stepY;
				side = 1;
			}
			if (map[mapX][mapY] > 0)
				hit = 1;
		}
		if (side == 0)
			perpWallDist = (mapX - game->vector.p_posX + (1 - stepX) / 2) / rayDirectionX;
		else
			perpWallDist = (mapY - game->vector.p_posY + (1 - stepY) / 2) / rayDirectionY;
		int lineHeight = (int)(480 / perpWallDist);
		int drawStart = (-lineHeight / 2) + (480 / 2);
		if (drawStart < 0)
			drawStart = 0;
		int drawEnd = (lineHeight / 2) + (480 / 2);
		if (drawEnd >= 480)
			drawEnd = 480 - 1;

		double wallX;
		if (side == 0)
			wallX = game->vector.p_posX + perpWallDist * rayDirectionY;
		else
			wallX = game->vector.p_posX + perpWallDist * rayDirectionX;
		wallX -= floor(wallX);

		int texX = (int)(wallX * (double)64);
		if (side == 0 && rayDirectionX > 0)
			texX = 64 - texX - 1;
		if (side == 1 && rayDirectionY < 0)
			texX = 64 - texX - 1;
		double step = 1.0 * 64 / lineHeight;
		double texPos = (drawStart - 480 / 2 + lineHeight / 2) * step;
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
	print_map(map);
	return GOOD_END;
}

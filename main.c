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

typedef struct s_mlx
{
	void	*ptr;
	void	*win;
	void	*img;

	char	*addr;
	int		pixel;
	int		size;
	int		endian;


}				t_mlx;

typedef struct s_vector
{
	double playerX;
	double playerY;
	double vectorDirectionX;
	double vectorDircetionY;
	double planeX;
	double planeY;
	double playerSpeed;
	double rotSpeed;
}				t_vector;

typedef struct s_game
{
	t_mlx	mlx;
	t_vector vector;
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

void	print_map(int map[][3])
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			printf("%d ", map[i][j]);
		}
		printf("\n");
	}
}

void	game_loop(t_game *game)
{
	int x;

	x = 0;
	while (x < WIN_WIDTH)
	{
		double cameraX = (2 * x / (double)(WIN_WIDTH)) - 1;
		
		double rayDirectionX = game->vector.vectorDirectionX + game->vector.planeX * cameraX;
		double rayDirectionY = game->vector.vectorDircetionY + game->vector.planeY * cameraX;

		int mapX = (int)(game->vector.playerX);
		int mapY = (int)(game->vector.playerY);

		double sideDistX;
		double sideDistY;

		double deltaDistX = fabs(1 / rayDirectionX);
		double deltaDistY = fabs(1 / rayDirectionY);

		double perpWallDist;

		int stepX;
		int stepY;

		int hit = 0;
		int side;


	}
}

int main()
{
	t_game	*game;

	// void	*img;
	// int		x;
	// int		y;

	// x = 64;
	// y = 64;

	int map[][3] = {
		{1, 1, 1},
		{1, 'N', 1},
		{1, 1, 1}
	};

	if (!(game = malloc(sizeof(t_game))))
	{
		write(1, ERROR, ft_strlen(ERROR));
		return BAD_END;
	}
	game->mlx.ptr = mlx_init();
	game->mlx.win = mlx_new_window(game->mlx.ptr, WIN_WIDTH, WIN_HEIGHT, "cub3D");

	// game->mlx.img = mlx_new_image(game->mlx.ptr, WIN_WIDTH, WIN_HEIGHT);
	// game->mlx.addr = mlx_get_data_addr(game->mlx.img, &game->mlx.pixel, &game->mlx.size, &game->mlx.endian);


	mlx_loop_hook(info.mlx, &game_loop, &game);
    mlx_hook(info.win, X_EVENT_KEY_PRESS, 0, &key_press, &info);

	// img = mlx_xpm_file_to_image(game->mlx.ptr, "./wall_w.xpm", &x, &y);
	// mlx_put_image_to_window(game->mlx.ptr, game->mlx.win, img, 0, 0);
	
	print_map(map);

	mlx_loop(game->mlx.ptr);
	free(game);
	return GOOD_END;
}

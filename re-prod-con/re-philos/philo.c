# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <unistd.h>
# include <sys/time.h>
# include <stdbool.h>

typedef struct  s_philo 
{
    int         id; 
    int         *meals_eaten;
    int         time_to_die;
    int         time_to_eat; 
    int         time_to_sleep;
    int         num_must_eat;
    long long   *last_meal_time;
    int         num_forks;
    int         num_philosophers;
    pthread_t   philo_thread;
    pthread_mutex_t *fork_mutex; 
    pthread_mutex_t *protection_mutex;
    long long   start_tme;
}               t_philo;

t_philo     *initialize_simulation(void);
t_philo	    *initialize_philosophers(t_philo *philo_info);
long long	*initialize_last_meal_time(int num_philosophers);
t_philo     *setup_health_monitor(t_philo *philosopher);
void        link_philophers_to_monitor(t_philo *philosophers, t_philo *monitor);
void        start_philosopher_threads(t_philo *philosophers);
void        clean_garbage(t_philo *philosophers, t_philo *monitor);
long long   ft_get_time(void);

int     main(void)
{
    t_philo     *philosophers;
    t_philo     *health_monitor;

    philosophers = initialize_simulation();
    if (!philosophers)
    {
        printf("Simulation initialization failed.\n");
        return (1);
    }
    health_monitor = setup_health_monitor(philosophers);
    if (!health_monitor)
    {
        printf("Health monitor initialization failed.\n");
        return (1;)
    }
    link_philophers_to_monitor(philosophers, health_monitor);
    start_philosopher_threads(philosophers);
    pthread_create(&health_monitor->philo_thread, NULL, 
        check_philosopher_health, health_monitor);
    pthread_join(health_monitor->philo_thread, NULL);
    return (clean_garbage(philosophers, health_monitor));
}

t_philo     *initialize_simulation(void)
{
    t_philo     *philo_info;
    t_philo     *philosophers_initialized;

    philo_info = malloc(sizeof(t_philo));
    if (!philo_info) return (NULL);

    philo_info->num_philosophers = 5;
    philo_info->time_to_die = 800;
    philo_info->time_to_eat = 200,
    philo_info->time_to_sleep = 200,
    philo_info->num_must_eat = -1;

    philosophers_initialized = initialize_philosophers(philo_info);

    // TO CONTINUE
}

t_philo	    *initialize_philosophers(t_philo *philo_info)
{
    t_philo         *philos_initialized;
    pthread_mutex_t *forks;
    long long       *last_meal_time; 

    philos_initialized = malloc(sizeof(t_philo) * philo_info->num_philosophers);
    if (!philos_initialized) return (NULL);
    last_meal_time = initialize_last_meal_time(philo_info->num_philosophers);
    initialize_philos_data(philos_initialized, philos_info, forks, last_meal_time);
    return (philos_initialized);
}

long long	*initialize_last_meal_time(int num_philosophers)
{
    int         i;
    long long   *last_meal_time;

    i = 0;
    last_meal_time = malloc(sizeof(long long) * num_philosophers);
    if (!last_meal_time) return (NULL);
    while (i < num_philosophers)
    {
        last_meal_time[i] = ft_get_time();
        i++;
    }
    return (last_meal_time);
}



long long	ft_get_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}
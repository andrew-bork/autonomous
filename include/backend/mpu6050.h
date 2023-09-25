#define MPU6050_DEFAULT_ADDR 0x68
#include <math/vector.h>
namespace mpu6050 {
	namespace accl_range{
		/**
		 * @brief Accelerometer range. g_2 means +- 2 gs. g = 9.81 m/s^2
		 * 
		 */
		enum accl_range {
			g_2 = 0b00,
			g_4 = 0b01,
			g_8 = 0b10,
			g_16 = 0b11,
		};
	};
	namespace gyro_range {
		/**
		 * @brief Gyroscope range. deg_250 means +- 250 degrees
		 * 
		 */
		enum gyro_range {
			deg_250 = 0b00,
			deg_500 = 0b01,
			deg_1000 = 0b10,
			deg_2000 = 0b11,
		};
	};
	namespace fsync {
		/**
		 * @brief i have no clue
		 * 
		 */
		enum fsync {
			input_dis,
			temp_out_l,
			gyro_x_out_l,
			gyro_y_out_l,
			gyro_z_out_l,
			accl_x_out_l,
			accl_y_out_l,
			accl_z_out_l,
		};
	};
	namespace dlpf{
		/**
		 * @brief Sets the coefficient of the internal low pass filter.
		 * 
		 */
		enum dlpf {
			hz_260,
			hz_184,
			hz_94,
			hz_44,
			hz_21,
			hz_10,
			hz_5, 
		};
	};
	namespace clk{
		/**
		 * @brief Sets the source of the mpu6050's internal clock
		 * 
		 */
		enum clk {
			int_oscl,
			x_gyro,
			y_gyro,
			z_gyro,
			ext_32kHz,
			ext_19MHz,
			reserved,
			stop
		};
	};


	void init();
	void init(int addr);

	bool is_awake();
	void wake_up();
	void sleep();

	void set_accl_set(accl_range::accl_range set);
	void set_gyro_set(gyro_range::gyro_range set);
	void set_clk(clk::clk set);
	void set_dlpf_bandwidth(dlpf::dlpf set);
	void set_fsync(fsync::fsync set);
	
	void set_offsets(int x_a, int y_a, int z_a, int x_g, int y_g, int z_g);

	void read_raw(int * data);
	
	void read_wo_offsets(double * data);

	void read(double * data);
	void read(math::vector& acceleration, math::vector& angular_velocity);
	

	int query_register(int reg);
	void set_register(int reg, int data);

	void calibrate(int n);

	void print_debug();

	void destroy();
};

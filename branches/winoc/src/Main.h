/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the top-level of Noxim
 */

#ifndef __NOXIMMAIN_H__
#define __NOXIMMAIN_H__

#include <cassert>
#include <systemc.h>
#include <vector>
using namespace std;

// Define the directions as numbers
#define DIRECTIONS             4
#define DIRECTION_NORTH        0
#define DIRECTION_EAST         1
#define DIRECTION_SOUTH        2
#define DIRECTION_WEST         3
#define DIRECTION_LOCAL        4
#define DIRECTION_WIRELESS     5

// Generic not reserved resource
#define NOT_RESERVED          -2

// To mark invalid or non exhistent values
#define NOT_VALID             -1

// Routing algorithms
#define ROUTING_XY             0
#define ROUTING_WEST_FIRST     1
#define ROUTING_NORTH_LAST     2
#define ROUTING_NEGATIVE_FIRST 3
#define ROUTING_ODD_EVEN       4
#define ROUTING_DYAD           5
#define ROUTING_FULLY_ADAPTIVE 8
#define ROUTING_TABLE_BASED    9
#define INVALID_ROUTING       -1

// Selection strategies
#define SEL_RANDOM             0
#define SEL_BUFFER_LEVEL       1
#define SEL_NOP                2
#define INVALID_SELECTION     -1

// Traffic distribution
#define TRAFFIC_RANDOM         0
#define TRAFFIC_TRANSPOSE1     1
#define TRAFFIC_TRANSPOSE2     2
#define TRAFFIC_HOTSPOT        3
#define TRAFFIC_TABLE_BASED    4
#define TRAFFIC_BIT_REVERSAL   5
#define TRAFFIC_SHUFFLE        6
#define TRAFFIC_BUTTERFLY      7
#define INVALID_TRAFFIC       -1

// Verbosity levels
#define VERBOSE_OFF            0
#define VERBOSE_LOW            1
#define VERBOSE_MEDIUM         2
#define VERBOSE_HIGH           3

// Default configuration can be overridden with command-line arguments
#define DEFAULT_VERBOSE_MODE                     VERBOSE_OFF
#define DEFAULT_TRACE_MODE                             false
#define DEFAULT_TRACE_FILENAME                            ""
#define DEFAULT_MESH_DIM_X                                 4
#define DEFAULT_MESH_DIM_Y                                 4
#define DEFAULT_BUFFER_DEPTH                               4
#define DEFAULT_MAX_PACKET_SIZE                           10
#define DEFAULT_MIN_PACKET_SIZE                            2
#define DEFAULT_ROUTING_ALGORITHM                 ROUTING_XY
#define DEFAULT_ROUTING_TABLE_FILENAME                    ""
#define DEFAULT_SELECTION_STRATEGY                SEL_RANDOM
#define DEFAULT_PACKET_INJECTION_RATE                   0.01
#define DEFAULT_PROBABILITY_OF_RETRANSMISSION           0.01
#define DEFAULT_TRAFFIC_DISTRIBUTION          TRAFFIC_RANDOM
#define DEFAULT_TRAFFIC_TABLE_FILENAME                    ""
#define DEFAULT_RESET_TIME                              1000
#define DEFAULT_SIMULATION_TIME                        10000
#define DEFAULT_STATS_WARM_UP_TIME        DEFAULT_RESET_TIME
#define DEFAULT_DETAILED                               false
#define DEFAULT_DYAD_THRESHOLD                           0.6
#define DEFAULT_MAX_VOLUME_TO_BE_DRAINED                   0
#define DEFAULT_ROUTER_PWR_FILENAME    "default_router.yaml"
#define DEFAULT_LOW_POWER_LINK_STRATEGY                false
#define DEFAULT_QOS                                      1.0
#define DEFAULT_SHOW_BUFFER_STATS                      false
#define DEFAULT_USE_WINOC                              false
#define DEFAULT_WINOC_CFG_FILENAME                        ""

// TODO by Fafa - this MUST be removed!!! Use only STL vectors instead!!!
#define MAX_STATIC_DIM 32
#define MAX_HUB_PORTS 4
#define MAX_HUB_CHANNELS 1
//#define MAX_HUBS 2

typedef unsigned int uint;

// GlobalParams -- used to forward configuration to every sub-block
struct GlobalParams {
    static int verbose_mode;
    static int trace_mode;
    static char trace_filename[128];
    static int mesh_dim_x;
    static int mesh_dim_y;
    static int buffer_depth;
    static int min_packet_size;
    static int max_packet_size;
    static int routing_algorithm;
    static char routing_table_filename[128];
    static int selection_strategy;
    static float packet_injection_rate;
    static float probability_of_retransmission;
    static int traffic_distribution;
    static char traffic_table_filename[128];
    static int simulation_time;
    static int stats_warm_up_time;
    static int rnd_generator_seed;
    static bool detailed;
    static vector <pair <int, double> > hotspots;
    static float dyad_threshold;
    static unsigned int max_volume_to_be_drained;
    static char router_power_filename[128];
    static bool low_power_link_strategy;
    static double qos;
    static bool show_buffer_stats;
    static bool use_winoc;
    static char winoc_cfg_fname[128];
};

// Coord -- XY coordinates type of the Tile inside the Mesh
class Coord {
  public:
    int x;			// X coordinate
    int y;			// Y coordinate

    inline bool operator ==(const Coord & coord) const {
	return (coord.x == x && coord.y == y);
}};

// FlitType -- Flit type enumeration
enum FlitType {
    FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL
};

// Payload -- Payload definition
struct Payload {
    sc_uint<32> data;	// Bus for the data to be exchanged

    inline bool operator ==(const Payload & payload) const {
	return (payload.data == data);
}};

// Packet -- Packet definition
struct Packet {
    int src_id;
    int dst_id;
    double timestamp;		// SC timestamp at packet generation
    int size;
    int flit_left;		// Number of remaining flits inside the packet
    bool use_low_voltage_path;

    // Constructors
    Packet() { }

    Packet(const int s, const int d, const double ts, const int sz) {
	make(s, d, ts, sz);
    }

    void make(const int s, const int d, const double ts, const int sz) {
	src_id = s;
	dst_id = d;
	timestamp = ts;
	size = sz;
	flit_left = sz;
	use_low_voltage_path = false;
    }
};

// RouteData -- data required to perform routing
struct RouteData {
    int current_id;
    int src_id;
    int dst_id;
    int dir_in;			// direction from which the packet comes from
};

struct ChannelStatus {
    int free_slots;		// occupied buffer slots
    bool available;		// 
    inline bool operator ==(const ChannelStatus & bs) const {
	return (free_slots == bs.free_slots && available == bs.available);
    };
};

// NoP_data -- NoP Data definition
struct NoP_data {
    int sender_id;
    ChannelStatus channel_status_neighbor[DIRECTIONS];

    inline bool operator ==(const NoP_data & nop_data) const {
	return (sender_id == nop_data.sender_id &&
		nop_data.channel_status_neighbor[0] ==
		channel_status_neighbor[0]
		&& nop_data.channel_status_neighbor[1] ==
		channel_status_neighbor[1]
		&& nop_data.channel_status_neighbor[2] ==
		channel_status_neighbor[2]
		&& nop_data.channel_status_neighbor[3] ==
		channel_status_neighbor[3]);
    };
};

// Flit -- Flit definition
struct Flit {
    int src_id;
    int dst_id;
    FlitType flit_type;	// The flit type (FLIT_TYPE_HEAD, FLIT_TYPE_BODY, FLIT_TYPE_TAIL)
    int sequence_no;		// The sequence number of the flit inside the packet
    Payload payload;	// Optional payload
    double timestamp;		// Unix timestamp at packet generation
    int hop_no;			// Current number of hops from source to destination
    bool use_low_voltage_path;

    inline bool operator ==(const Flit & flit) const {
	return (flit.src_id == src_id && flit.dst_id == dst_id
		&& flit.flit_type == flit_type
		&& flit.sequence_no == sequence_no
		&& flit.payload == payload && flit.timestamp == timestamp
		&& flit.hop_no == hop_no
		&& flit.use_low_voltage_path == use_low_voltage_path);
}};

// Output overloading

inline ostream & operator <<(ostream & os, const Flit & flit)
{

    if (GlobalParams::verbose_mode == VERBOSE_HIGH) {

	os << "### FLIT ###" << endl;
	os << "Source Tile[" << flit.src_id << "]" << endl;
	os << "Destination Tile[" << flit.dst_id << "]" << endl;
	switch (flit.flit_type) {
	case FLIT_TYPE_HEAD:
	    os << "Flit Type is HEAD" << endl;
	    break;
	case FLIT_TYPE_BODY:
	    os << "Flit Type is BODY" << endl;
	    break;
	case FLIT_TYPE_TAIL:
	    os << "Flit Type is TAIL" << endl;
	    break;
	}
	os << "Sequence no. " << flit.sequence_no << endl;
	os << "Payload printing not implemented (yet)." << endl;
	os << "Unix timestamp at packet generation " << flit.
	    timestamp << endl;
	os << "Total number of hops from source to destination is " <<
	    flit.hop_no << endl;
    } else {
	os << "[type: ";
	switch (flit.flit_type) {
	case FLIT_TYPE_HEAD:
	    os << "H";
	    break;
	case FLIT_TYPE_BODY:
	    os << "B";
	    break;
	case FLIT_TYPE_TAIL:
	    os << "T";
	    break;
	}

	os << ", seq: " << flit.sequence_no << ", " << flit.src_id << "-->" << flit.dst_id << "]";
    }

    return os;
}

inline ostream & operator <<(ostream & os,
			     const ChannelStatus & status)
{
    char msg;
    if (status.available)
	msg = 'A';
    else
	msg = 'N';
    os << msg << "(" << status.free_slots << ")";
    return os;
}

inline ostream & operator <<(ostream & os, const NoP_data & NoP_data)
{
    os << "      NoP data from [" << NoP_data.sender_id << "] [ ";

    for (int j = 0; j < DIRECTIONS; j++)
	os << NoP_data.channel_status_neighbor[j] << " ";

    cout << "]" << endl;
    return os;
}

inline ostream & operator <<(ostream & os, const Coord & coord)
{
    os << "(" << coord.x << "," << coord.y << ")";

    return os;
}

// Trace overloading

inline void sc_trace(sc_trace_file * &tf, const Flit & flit, string & name)
{
    sc_trace(tf, flit.src_id, name + ".src_id");
    sc_trace(tf, flit.dst_id, name + ".dst_id");
    sc_trace(tf, flit.sequence_no, name + ".sequence_no");
    sc_trace(tf, flit.timestamp, name + ".timestamp");
    sc_trace(tf, flit.hop_no, name + ".hop_no");
}

inline void sc_trace(sc_trace_file * &tf, const NoP_data & NoP_data, string & name)
{
    sc_trace(tf, NoP_data.sender_id, name + ".sender_id");
}

inline void sc_trace(sc_trace_file * &tf, const ChannelStatus & bs, string & name)
{
    sc_trace(tf, bs.free_slots, name + ".free_slots");
    sc_trace(tf, bs.available, name + ".available");
}

// Misc common functions

inline Coord id2Coord(int id)
{
    Coord coord;

    coord.x = id % GlobalParams::mesh_dim_x;
    coord.y = id / GlobalParams::mesh_dim_x;

    assert(coord.x < GlobalParams::mesh_dim_x);
    assert(coord.y < GlobalParams::mesh_dim_y);

    return coord;
}

inline int coord2Id(const Coord & coord)
{
    int id = (coord.y * GlobalParams::mesh_dim_x) + coord.x;

    assert(id < GlobalParams::mesh_dim_x * GlobalParams::mesh_dim_y);

    return id;
}

inline bool SameRadioHub(int id1, int id2)
{
    // TODO WIRELSS: replace with actual choice based on topology

    if (id1==0 && id2==3)
	return false;

    return true;
}


#endif

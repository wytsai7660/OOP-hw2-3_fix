#include <climits>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <utility>
#include <vector>


using namespace std;

#define SET(type, var_name) void set_##var_name(type _##var_name) { (var_name) = _##var_name; }
#define SET_WITH_NAME(setter_name, type, var_name) void setter_name(type _##var_name) { (var_name) = _##var_name; }
#define GET(type, var_name) type get_##var_name() const { return var_name; }
#define GET_WITH_NAME(getter_name, type, var_name) type getter_name() const { return var_name; }

class header;
class payload;
class packet;
class node;
class event;
class link; // new

// for simplicity, we use a const int to simulate the delay
// if you want to simulate the more details, you should revise it to be a class
const unsigned int ONE_HOP_DELAY = 10;
const unsigned int BROCAST_ID = UINT_MAX;

// BROCAST_ID means that all neighbors are receivers; UINT_MAX is the maximum value of unsigned int

class header {
    public:
        virtual ~header() = default;

        SET(unsigned int, src_ID)
        SET(unsigned int, dst_ID)
        SET(unsigned int, pre_ID)
        SET(unsigned int, nex_ID)
        GET(unsigned int, src_ID)
        GET(unsigned int, dst_ID)
        GET(unsigned int, pre_ID)
        GET(unsigned int, nex_ID)
        virtual string type() = 0;

        // factory concept: generate a header
        class header_generator {
                // lock the copy constructor
                header_generator(header_generator &) = default;
                // store all possible types of header
                static map<string,header_generator*> prototypes;
            protected:
                // allow derived class to use it
                header_generator() = default;
                // after you create a new header type, please register the factory of this header type by this function
                void register_header_type(header_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your header
                virtual header* generate() = 0 ;
            public:
                // you have to implement your own type() to return your header type
        	    virtual string type() = 0 ;
        	    // this function is used to generate any type of header derived
        	    static header * generate (const string &type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such header type" << '\n'; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered header types: " << '\n';
            	    for (map<string,header::header_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++) {
            	        cout << it->second->type() << '\n';
                    }
            	}
            	virtual ~header_generator() = default;
        };

    protected:
        header():src_ID(BROCAST_ID),dst_ID(BROCAST_ID),pre_ID(BROCAST_ID),nex_ID(BROCAST_ID){} // this constructor cannot be directly called by users

    private:
        unsigned int src_ID;
        unsigned int dst_ID;
        unsigned int pre_ID;
        unsigned int nex_ID;
        header(header&){} // this constructor cannot be directly called by users
};
map<string,header::header_generator*> header::header_generator::prototypes;

class IoT_data_header : public header{
        IoT_data_header(IoT_data_header&){} // cannot be called by users

    protected:
        IoT_data_header() = default; // this constructor cannot be directly called by users

    public:
        ~IoT_data_header() override = default;
        string type() override { return "IoT_data_header"; }

        // IoT_data_header_generator is derived from header_generator to generate a header
        class IoT_data_header_generator : public header_generator{
                static IoT_data_header_generator sample;
                // this constructor is only for sample to register this header type
                IoT_data_header_generator() { /*cout << "IoT_data_header registered" << '\n';*/ register_header_type(&sample); }
            protected:
                header * generate() override {
                    // cout << "IoT_data_header generated" << '\n';
                    return new IoT_data_header;
                }
            public:
                string type() override { return "IoT_data_header";}
                ~IoT_data_header_generator() override = default;

        };
};
IoT_data_header::IoT_data_header_generator IoT_data_header::IoT_data_header_generator::sample;

class IoT_ctrl_header : public header{
        IoT_ctrl_header(IoT_ctrl_header&){} // cannot be called by users

    protected:
        IoT_ctrl_header() = default; // this constructor cannot be directly called by users

    public:
        ~IoT_ctrl_header() override = default;
        string type() override { return "IoT_ctrl_header"; }

        // IoT_ctrl_header_generator is derived from header_generator to generate a header
        class IoT_ctrl_header_generator : public header_generator{
                static IoT_ctrl_header_generator sample;
                // this constructor is only for sample to register this header type
                IoT_ctrl_header_generator() { /*cout << "IoT_ctrl_header registered" << '\n';*/ register_header_type(&sample); }
            protected:
                header * generate() override {
                    // cout << "IoT_ctrl_header generated" << '\n';
                    return new IoT_ctrl_header;
                }
            public:
                string type() override { return "IoT_ctrl_header";}
                ~IoT_ctrl_header_generator() override = default;

        };
};
IoT_ctrl_header::IoT_ctrl_header_generator IoT_ctrl_header::IoT_ctrl_header_generator::sample;


class AGG_ctrl_header : public header{
        AGG_ctrl_header(AGG_ctrl_header&){} // cannot be called by users

    protected:
        AGG_ctrl_header() = default; // this constructor cannot be directly called by users

    public:
        ~AGG_ctrl_header() override = default;
        string type() override { return "AGG_ctrl_header"; }

        // AGG_ctrl_header_generator is derived from header_generator to generate a header
        class AGG_ctrl_header_generator : public header_generator{
                static AGG_ctrl_header_generator sample;
                // this constructor is only for sample to register this header type
                AGG_ctrl_header_generator() { /*cout << "AGG_ctrl_header registered" << '\n';*/ register_header_type(&sample); }
            protected:
                header * generate() override {
                    // cout << "AGG_ctrl_header generated" << '\n';
                    return new AGG_ctrl_header;
                }
            public:
                string type() override { return "AGG_ctrl_header";}
                ~AGG_ctrl_header_generator() override = default;

        };
};
AGG_ctrl_header::AGG_ctrl_header_generator AGG_ctrl_header::AGG_ctrl_header_generator::sample;


class DIS_ctrl_header : public header{
        DIS_ctrl_header(DIS_ctrl_header&){} // cannot be called by users

    protected:
        DIS_ctrl_header() = default; // this constructor cannot be directly called by users

    public:
        ~DIS_ctrl_header() override = default;
        string type() override { return "DIS_ctrl_header"; }

        // DIS_ctrl_header_generator is derived from header_generator to generate a header
        class DIS_ctrl_header_generator : public header_generator{
                static DIS_ctrl_header_generator sample;
                // this constructor is only for sample to register this header type
                DIS_ctrl_header_generator() { /*cout << "DIS_ctrl_header registered" << '\n';*/ register_header_type(&sample); }
            protected:
                header * generate() override {
                    // cout << "DIS_ctrl_header generated" << '\n';
                    return new DIS_ctrl_header;
                }
            public:
                string type() override { return "DIS_ctrl_header";}
                ~DIS_ctrl_header_generator() override = default;

        };
};
DIS_ctrl_header::DIS_ctrl_header_generator DIS_ctrl_header::DIS_ctrl_header_generator::sample;

class payload {
        payload(payload&){} // this constructor cannot be directly called by users

        string msg;

    protected:
        payload() = default;
    public:
        virtual ~payload() = default;
        virtual string type() = 0;

        SET(string, msg)
        GET(string, msg)

        class payload_generator {
                // lock the copy constructor
                payload_generator(payload_generator &) = default;
                // store all possible types of header
                static map<string,payload_generator*> prototypes;
            protected:
                // allow derived class to use it
                payload_generator() = default;
                // after you create a new payload type, please register the factory of this payload type by this function
                void register_payload_type(payload_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your payload
                virtual payload* generate() = 0;
            public:
                // you have to implement your own type() to return your header type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of header derived
        	    static payload * generate (const string &type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such payload type" << '\n'; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered payload types: " << '\n';
            	    for (map<string,payload::payload_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++) {
            	        cout << it->second->type() << '\n';
                    }
            	}
            	virtual ~payload_generator() = default;
        };
};
map<string,payload::payload_generator*> payload::payload_generator::prototypes;


class IoT_data_payload : public payload {
        IoT_data_payload(IoT_data_payload&){}

    protected:
        IoT_data_payload() = default; // this constructor cannot be directly called by users
    public:
        ~IoT_data_payload() override = default;

        string type() override { return "IoT_data_payload"; }

        // IoT_data_payload is derived from payload_generator to generate a payload
        class IoT_data_payload_generator : public payload_generator{
                static IoT_data_payload_generator sample;
                // this constructor is only for sample to register this payload type
                IoT_data_payload_generator() { /*cout << "IoT_data_payload registered" << '\n';*/ register_payload_type(&sample); }
            protected:
                payload * generate() override {
                    // cout << "IoT_data_payload generated" << '\n';
                    return new IoT_data_payload;
                }
            public:
                string type() override { return "IoT_data_payload";}
                ~IoT_data_payload_generator() override = default;
        };
};
IoT_data_payload::IoT_data_payload_generator IoT_data_payload::IoT_data_payload_generator::sample;

class IoT_ctrl_payload : public payload {
        IoT_ctrl_payload(IoT_ctrl_payload & s): counter (s.counter) {}

        unsigned int counter ;

    protected:
        IoT_ctrl_payload(): counter (0) {} // this constructor cannot be directly called by users
    public:
        ~IoT_ctrl_payload() override = default;

        void increase() { counter ++; } // used to increase the counter
        GET(unsigned int, counter) // used to get the value of counter

        string type() override { return "IoT_ctrl_payload"; }

        // IoT_data_payload is derived from payload_generator to generate a payload
        class IoT_ctrl_payload_generator : public payload_generator{
                static IoT_ctrl_payload_generator sample;
                // this constructor is only for sample to register this payload type
                IoT_ctrl_payload_generator() { /*cout << "IoT_ctrl_payload registered" << '\n';*/ register_payload_type(&sample); }
            protected:
                payload * generate() override {
                    // cout << "IoT_ctrl_payload generated" << '\n';
                    return new IoT_ctrl_payload;
                }
            public:
                string type() override { return "IoT_ctrl_payload";}
                ~IoT_ctrl_payload_generator() override = default;
        };
};
IoT_ctrl_payload::IoT_ctrl_payload_generator IoT_ctrl_payload::IoT_ctrl_payload_generator::sample;


class AGG_ctrl_payload : public payload {
        AGG_ctrl_payload(AGG_ctrl_payload & s) {}

        // unsigned int counter ;

    protected:
        AGG_ctrl_payload() = default; // this constructor cannot be directly called by users
    public:
        ~AGG_ctrl_payload() override = default;

        // void increase() { counter ++; } // used to increase the counter
        // GET(getCounter,unsigned int,counter); // used to get the value of counter

        string type() override { return "AGG_ctrl_payload"; }

        // IoT_data_payload is derived from payload_generator to generate a payload
        class AGG_ctrl_payload_generator : public payload_generator{
                static AGG_ctrl_payload_generator sample;
                // this constructor is only for sample to register this payload type
                AGG_ctrl_payload_generator() { /*cout << "AGG_ctrl_payload registered" << '\n';*/ register_payload_type(&sample); }
            protected:
                payload * generate() override {
                    // cout << "AGG_ctrl_payload generated" << '\n';
                    return new AGG_ctrl_payload;
                }
            public:
                string type() override { return "AGG_ctrl_payload";}
                ~AGG_ctrl_payload_generator() override = default;
        };
};
AGG_ctrl_payload::AGG_ctrl_payload_generator AGG_ctrl_payload::AGG_ctrl_payload_generator::sample;


class DIS_ctrl_payload : public payload {
        DIS_ctrl_payload(DIS_ctrl_payload & s): parent(s.parent) {}

        // unsigned int counter ;
        unsigned int parent;

    protected:
        DIS_ctrl_payload(int _parent=0): parent (_parent) {} // this constructor cannot be directly called by users
    public:
        ~DIS_ctrl_payload() override = default;

        // void increase() { counter ++; } // used to increase the counter
        SET(unsigned int, parent)
        GET(unsigned int, parent) // used to get the value of counter

        string type() override { return "DIS_ctrl_payload"; }

        // IoT_data_payload is derived from payload_generator to generate a payload
        class DIS_ctrl_payload_generator : public payload_generator{
                static DIS_ctrl_payload_generator sample;
                // this constructor is only for sample to register this payload type
                DIS_ctrl_payload_generator() { /*cout << "DIS_ctrl_payload registered" << '\n';*/ register_payload_type(&sample); }
            protected:
                payload * generate() override {
                    // cout << "DIS_ctrl_payload generated" << '\n';
                    return new DIS_ctrl_payload;
                }
            public:
                string type() override { return "DIS_ctrl_payload";}
                ~DIS_ctrl_payload_generator() override = default;
        };
};
DIS_ctrl_payload::DIS_ctrl_payload_generator DIS_ctrl_payload::DIS_ctrl_payload_generator::sample;


class packet{
        // a packet usually contains a header and a payload
        header *hdr;
        payload *pld;
        unsigned int p_id;
        static unsigned int last_packet_id ;

        packet(packet &) {}
        static int live_packet_num ;
    protected:
        // these constructors cannot be directly called by users
        packet(): hdr(nullptr), pld(nullptr) { p_id=last_packet_id++; live_packet_num ++; }
        packet(const string &_hdr, const string &_pld, bool rep = false, unsigned int rep_id = 0) {
            if (! rep )  { // a duplicated packet does not have a new packet id
                p_id = last_packet_id ++;
            }
            else {
                p_id = rep_id;
            }
            hdr = header::header_generator::generate(_hdr);
            pld = payload::payload_generator::generate(_pld);
            live_packet_num ++;
        }
    public:
        virtual ~packet(){
            // cout << "packet destructor begin" << '\n';
            if (hdr != nullptr) {
                delete hdr;
            }
            if (pld != nullptr) {
                delete pld;
            }
            live_packet_num --;
            // cout << "packet destructor end" << '\n';
        }

        SET_WITH_NAME(set_header, header *, hdr)
        GET_WITH_NAME(get_header, header *, hdr)
        SET_WITH_NAME(set_payload, payload *, pld)
        GET_WITH_NAME(get_payload, payload *, pld)
        GET_WITH_NAME(get_packet_ID, unsigned int, p_id)

        static void discard ( packet* &p ) {
            // cout << "checking" << '\n';
            if (p != nullptr) {
                // cout << "discarding" << '\n';
                // cout << p->type() << '\n';
                delete p;
                // cout << "discarded" << '\n';
            }
            p = nullptr;
            // cout << "checked" << '\n';
        }
        virtual string type () = 0;
        // you can define your own packet's addition_information
        // to print more information for recv_event and send_event
        virtual string addition_information () { return ""; }

        static int get_live_packet_num () { return live_packet_num; }

        class packet_generator {
                // lock the copy constructor
                packet_generator(packet_generator &) = default;
                // store all possible types of packet
                static map<string,packet_generator*> prototypes;
            protected:
                // allow derived class to use it
                packet_generator() = default;
                // after you create a new packet type, please register the factory of this payload type by this function
                void register_packet_type(packet_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your payload
                virtual packet* generate ( packet *p = nullptr) = 0;
            public:
                // you have to implement your own type() to return your packet type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of packet derived
        	    static packet * generate (const string &type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such packet type" << '\n'; // otherwise
            		return nullptr;
            	}
            	static packet * replicate (packet *p) {
            	    if(prototypes.find(p->type()) != prototypes.end()){ // if this type derived exists
            			return prototypes[p->type()]->generate(p); // generate it!!
            		}
            		std::cerr << "no such packet type" << '\n'; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered packet types: " << '\n';
            	    for (map<string,packet::packet_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++) {
            	        cout << it->second->type() << '\n';
                    }
            	}
            	virtual ~packet_generator() = default;
        };
};
map<string,packet::packet_generator*> packet::packet_generator::prototypes;
unsigned int packet::last_packet_id = 0 ;
int packet::live_packet_num = 0;


// this packet is used to transmit the data
class IoT_data_packet: public packet {
        IoT_data_packet(IoT_data_packet &) {}

    protected:
        IoT_data_packet() = default; // this constructor cannot be directly called by users
        IoT_data_packet(packet*p): packet(p->get_header()->type(), p->get_payload()->type(), true, p->get_packet_ID()) {
            *(dynamic_cast<IoT_data_header*>(this->get_header())) = *(dynamic_cast<IoT_data_header*> (p->get_header()));
            *(dynamic_cast<IoT_data_payload*>(this->get_payload())) = *(dynamic_cast<IoT_data_payload*> (p->get_payload()));
            //DFS_path = (dynamic_cast<IoT_data_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<IoT_data_header*>(p))->isVisited;
        } // for duplicate
        IoT_data_packet(const string &_h, const string &_p): packet(_h, _p) {}

    public:
        ~IoT_data_packet() override = default;
        string type() override { return "IoT_data_packet"; }

        // IoT_data_packet is derived from packet_generator to generate a pub packet
        class IoT_data_packet_generator : public packet_generator{
                static IoT_data_packet_generator sample;
                // this constructor is only for sample to register this packet type
                IoT_data_packet_generator() { /*cout << "IoT_data_packet registered" << '\n';*/ register_packet_type(&sample); }
            protected:
                packet *generate (packet *p = nullptr) override {
                    // cout << "IoT_data_packet generated" << '\n';
                    if ( nullptr == p ) {
                        return new IoT_data_packet("IoT_data_header","IoT_data_payload");
                    }
                    else {
                        return new IoT_data_packet(p); // duplicate
                    }
                }
            public:
                string type() override { return "IoT_data_packet";}
                ~IoT_data_packet_generator() override = default;
        };
};
IoT_data_packet::IoT_data_packet_generator IoT_data_packet::IoT_data_packet_generator::sample;

// this packet type is used to conduct distributed BFS
class IoT_ctrl_packet: public packet {
        IoT_ctrl_packet(IoT_ctrl_packet &) {}

    protected:
        IoT_ctrl_packet() = default; // this constructor cannot be directly called by users
        IoT_ctrl_packet(packet*p): packet(p->get_header()->type(), p->get_payload()->type(), true, p->get_packet_ID()) {
            *(dynamic_cast<IoT_ctrl_header*>(this->get_header())) = *(dynamic_cast<IoT_ctrl_header*> (p->get_header()));
            *(dynamic_cast<IoT_ctrl_payload*>(this->get_payload())) = *(dynamic_cast<IoT_ctrl_payload*> (p->get_payload()));
            //DFS_path = (dynamic_cast<IoT_ctrl_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<IoT_ctrl_header*>(p))->isVisited;
        } // for duplicate
        IoT_ctrl_packet(const string &_h, const string &_p): packet(_h, _p) {}

    public:
        ~IoT_ctrl_packet() override = default;
        string type() override { return "IoT_ctrl_packet"; }
        string addition_information() override {
            unsigned int counter = (dynamic_cast<IoT_ctrl_payload*>(this->get_payload()))->get_counter();
            // cout << counter << '\n';
            return " counter " + to_string(counter);
        }

        // IoT_ctrl_packet is derived from packet_generator to generate a pub packet
        class IoT_ctrl_packet_generator : public packet_generator{
                static IoT_ctrl_packet_generator sample;
                // this constructor is only for sample to register this packet type
                IoT_ctrl_packet_generator() { /*cout << "IoT_ctrl_packet registered" << '\n';*/ register_packet_type(&sample); }
            protected:
                packet *generate (packet *p = nullptr) override {
                    // cout << "IoT_ctrl_packet generated" << '\n';
                    if ( nullptr == p ) {
                        return new IoT_ctrl_packet("IoT_ctrl_header","IoT_ctrl_payload");
                    }
                    else {
                        return new IoT_ctrl_packet(p); // duplicate
                    }
                }
            public:
                string type() override { return "IoT_ctrl_packet";}
                ~IoT_ctrl_packet_generator() override = default;
        };
};
IoT_ctrl_packet::IoT_ctrl_packet_generator IoT_ctrl_packet::IoT_ctrl_packet_generator::sample;



// this packet type is used to transmit each device's nblist to the sink
class AGG_ctrl_packet: public packet {
        AGG_ctrl_packet(AGG_ctrl_packet &) {}

    protected:
        AGG_ctrl_packet() = default; // this constructor cannot be directly called by users
        AGG_ctrl_packet(packet*p): packet(p->get_header()->type(), p->get_payload()->type(), true, p->get_packet_ID()) {
            *(dynamic_cast<AGG_ctrl_header*>(this->get_header())) = *(dynamic_cast<AGG_ctrl_header*> (p->get_header()));
            *(dynamic_cast<AGG_ctrl_payload*>(this->get_payload())) = *(dynamic_cast<AGG_ctrl_payload*> (p->get_payload()));
            //DFS_path = (dynamic_cast<AGG_ctrl_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<AGG_ctrl_header*>(p))->isVisited;
        } // for duplicate
        AGG_ctrl_packet(const string &_h, const string &_p): packet(_h, _p) {}

    public:
        ~AGG_ctrl_packet() override = default;
        string type() override { return "AGG_ctrl_packet"; }

        // virtual string addition_information() {
        //     string msg = (dynamic_cast<AGG_ctrl_payload*>(this->get_payload()))->getMsg();
        //     return " msg " + msg;
        // }

        // AGG_ctrl_packet is derived from packet_generator to generate a pub packet
        class AGG_ctrl_packet_generator : public packet_generator{
                static AGG_ctrl_packet_generator sample;
                // this constructor is only for sample to register this packet type
                AGG_ctrl_packet_generator() { /*cout << "AGG_ctrl_packet registered" << '\n';*/ register_packet_type(&sample); }
            protected:
                packet *generate (packet *p = nullptr) override {
                    // cout << "AGG_ctrl_packet generated" << '\n';
                    if ( nullptr == p ) {
                        return new AGG_ctrl_packet("AGG_ctrl_header","AGG_ctrl_payload");
                    }
                    else {
                        return new AGG_ctrl_packet(p); // duplicate
                    }
                }
            public:
                string type() override { return "AGG_ctrl_packet";}
                ~AGG_ctrl_packet_generator() override = default;
        };
};
AGG_ctrl_packet::AGG_ctrl_packet_generator AGG_ctrl_packet::AGG_ctrl_packet_generator::sample;


// this packet type is used to transmit the new parent to each device
class DIS_ctrl_packet: public packet {
        DIS_ctrl_packet(DIS_ctrl_packet &) {}

    protected:
        DIS_ctrl_packet() = default; // this constructor cannot be directly called by users
        DIS_ctrl_packet(packet*p): packet(p->get_header()->type(), p->get_payload()->type(), true, p->get_packet_ID()) {
            *(dynamic_cast<DIS_ctrl_header*>(this->get_header())) = *(dynamic_cast<DIS_ctrl_header*> (p->get_header()));
            *(dynamic_cast<DIS_ctrl_payload*>(this->get_payload())) = *(dynamic_cast<DIS_ctrl_payload*> (p->get_payload()));
            //DFS_path = (dynamic_cast<DIS_ctrl_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<DIS_ctrl_header*>(p))->isVisited;
        } // for duplicate
        DIS_ctrl_packet(const string &_h, const string &_p): packet(_h, _p) {}

    public:
        ~DIS_ctrl_packet() override = default;
        string type() override { return "DIS_ctrl_packet"; }
        string addition_information() override {
            unsigned int parent = (dynamic_cast<DIS_ctrl_payload*>(this->get_payload()))->get_parent();
            // cout << counter << '\n';
            return " parent " + to_string(parent);
        }

        // DIS_ctrl_packet is derived from packet_generator to generate a pub packet
        class DIS_ctrl_packet_generator : public packet_generator{
                static DIS_ctrl_packet_generator sample;
                // this constructor is only for sample to register this packet type
                DIS_ctrl_packet_generator() { /*cout << "DIS_ctrl_packet registered" << '\n';*/ register_packet_type(&sample); }
            protected:
                packet *generate (packet *p = nullptr) override {
                    // cout << "DIS_ctrl_packet generated" << '\n';
                    if ( nullptr == p ) {
                        return new DIS_ctrl_packet("DIS_ctrl_header","DIS_ctrl_payload");
                    }
                    else {
                        return new DIS_ctrl_packet(p); // duplicate
                    }
                }
            public:
                string type() override { return "DIS_ctrl_packet";}
                ~DIS_ctrl_packet_generator() override = default;
        };
};
DIS_ctrl_packet::DIS_ctrl_packet_generator DIS_ctrl_packet::DIS_ctrl_packet_generator::sample;




class node {
        // all nodes created in the program
        static map<unsigned int, node*> id_node_table;

        unsigned int id;
        map<unsigned int,bool> phy_neighbors;

    protected:
        node(node&){} // this constructor should not be used
        node() = default; // this constructor should not be used
        node(unsigned int _id): id(_id) { id_node_table[_id] = this; }
    public:
        virtual ~node() { // erase the node
            id_node_table.erase (id) ;
        }
        virtual string type() = 0; // please define it in your derived node class

        void add_phy_neighbor (unsigned int _id, const string &link_type = "simple_link"); // we only add a directed link from id to _id
        void del_phy_neighbor (unsigned int _id); // we only delete a directed link from id to _id

        // you can use the function to get the node's neigbhors at this time
        // but in the project 3, you are not allowed to use this function
        const map<unsigned int,bool> & getPhyNeighbors () {
            return phy_neighbors;
        }


        void recv (packet *p) {
            packet *tp = p;
            recv_handler(tp);
            packet::discard(p);
        } // the packet will be directly deleted after the handler
        void send (packet *p);

        // receive the packet and do something; this is a pure virtual function
        virtual void recv_handler(packet *p) = 0;
        void send_handler(packet *P);

        static node * id_to_node (unsigned int _id) { return ((id_node_table.find(_id)!=id_node_table.end()) ? id_node_table[_id]: nullptr) ; }
        GET_WITH_NAME(getNodeID, unsigned int, id)

        static void del_node (unsigned int _id) {
            if (id_node_table.find(_id) != id_node_table.end()) {
                id_node_table.erase(_id);
            }
        }
        static unsigned int getNodeNum () { return id_node_table.size(); }

        class node_generator {
                // lock the copy constructor
                node_generator(node_generator &) = default;
                // store all possible types of node
                static map<string,node_generator*> prototypes;
            protected:
                // allow derived class to use it
                node_generator() = default;
                // after you create a new node type, please register the factory of this node type by this function
                void register_node_type(node_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your node
                virtual node* generate(unsigned int _id) = 0;
            public:
                // you have to implement your own type() to return your node type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of node derived
        	    static node * generate (const string &type, unsigned int _id) {
        	        if(id_node_table.find(_id)!=id_node_table.end()){
        	            std::cerr << "duplicate node id" << '\n'; // node id is duplicated
        	            return nullptr;
        	        }
        	        else if ( BROCAST_ID == _id ) {
        	            cerr << "BROCAST_ID cannot be used" << '\n';
        	            return nullptr;
        	        }
            		else if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            		    node * created_node = prototypes[type]->generate(_id);
            			return created_node; // generate it!!
            		}
            		std::cerr << "no such node type" << '\n'; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered node types: " << '\n';
            	    for (map<string,node::node_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++) {
            	        cout << it->second->type() << '\n';
                    }
            	}
            	virtual ~node_generator() = default;
        };
};
map<string,node::node_generator*> node::node_generator::prototypes;
map<unsigned int,node*> node::id_node_table;

class IoT_device: public node {
        // map<unsigned int,bool> one_hop_neighbors; // you can use this variable to record the node's 1-hop neighbors

        bool hi; // this is used for example; you can remove it when doing hw2

    protected:
        IoT_device() = default; // it should not be used
        IoT_device(IoT_device&) {} // it should not be used
        IoT_device(unsigned int _id): node(_id), hi(false) {} // this constructor cannot be directly called by users

    public:
        ~IoT_device() override = default;
        string type() override { return "IoT_device"; }

        // please define recv_handler function to deal with the incoming packet
        void recv_handler (packet *p) override;

        // void add_one_hop_neighbor (unsigned int n_id) { one_hop_neighbors[n_id] = true; }
        // unsigned int get_one_hop_neighbor_num () { return one_hop_neighbors.size(); }

        // IoT_device is derived from node_generator to generate a node
        class IoT_device_generator : public node_generator{
                static IoT_device_generator sample;
                // this constructor is only for sample to register this node type
                IoT_device_generator() { /*cout << "IoT_device registered" << '\n';*/ register_node_type(&sample); }
            protected:
                node * generate(unsigned int _id) override { /*cout << "IoT_device generated" << '\n';*/ return new IoT_device(_id); }
            public:
                string type() override { return "IoT_device";}
                ~IoT_device_generator() override = default;
        };
};
IoT_device::IoT_device_generator IoT_device::IoT_device_generator::sample;

class mycomp {
    bool reverse;

    public:
        mycomp(const bool& revparam = false) { reverse=revparam ; }
        bool operator() (const event* lhs, const event* rhs) const;
};

class event {
        event(event*&){} // this constructor cannot be directly called by users
        static priority_queue < event*, vector < event* >, mycomp > events;
        static unsigned int cur_time; // timer
        static unsigned int end_time;

        // get the next event
        static event * get_next_event() ;
        static void add_event (event *e) { events.push(e); }
        static hash<string> event_seq;

    protected:
        unsigned int trigger_time;

        event() = default; // it should not be used
        event(unsigned int _trigger_time): trigger_time(_trigger_time) {}

    public:
        virtual void trigger()=0;
        virtual ~event() = default;

        virtual unsigned int event_priority() const = 0;
        unsigned int get_hash_value(const string &string_for_hash) const {
            unsigned int priority = event_seq (string_for_hash);
            return priority;
        }

        static void flush_events (); // only for debug

        GET(unsigned int, trigger_time)

        static void start_simulate( unsigned int _end_time ); // the function is used to start the simulation

        static unsigned int getCurTime() { return cur_time ; }
        static void getCurTime(unsigned int _cur_time) { cur_time = _cur_time; }
        // static unsigned int getEndTime() { return end_time ; }
        // static void getEndTime(unsigned int _end_time) { end_time = _end_time; }

        virtual void print () const = 0; // the function is used to print the event information

        class event_generator{
                // lock the copy constructor
                event_generator(event_generator &) = default;
                // store all possible types of event
                static map<string,event_generator*> prototypes;
            protected:
                // allow derived class to use it
                event_generator() = default;
                // after you create a new event type, please register the factory of this event type by this function
                void register_event_type(event_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your event
                virtual event* generate(unsigned int _trigger_time, void * data) = 0;
            public:
                // you have to implement your own type() to return your event type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of event derived
        	    static event * generate (const string &type, unsigned int _trigger_time, void *data) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            		    event * e = prototypes[type]->generate(_trigger_time, data);
            		    add_event(e);
            		    return e; // generate it!!
            		}
            		std::cerr << "no such event type" << '\n'; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered event types: " << '\n';
            	    for (map<string,event::event_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++) {
            	        cout << it->second->type() << '\n';
                    }
            	}
            	virtual ~event_generator() = default;
        };
};
map<string,event::event_generator*> event::event_generator::prototypes;
priority_queue < event*, vector< event* >, mycomp > event::events;
hash<string> event::event_seq;

unsigned int event::cur_time = 0;
unsigned int event::end_time = 0;

void event::flush_events()
{
    cout << "**flush begin" << '\n';
    while ( ! events.empty() ) {
        cout << setw(11) << events.top()->trigger_time << ": " << setw(11) << events.top()->event_priority() << '\n';
        delete events.top();
        events.pop();
    }
    cout << "**flush end" << '\n';
}
event * event::get_next_event() {
    if(events.empty()) {
        return nullptr;
    }
    event * e = events.top();
    events.pop();
    // cout << events.size() << " events remains" << '\n';
    return e;
}
void event::start_simulate(unsigned int _end_time) {
    if (_end_time<0) {
        cerr << "you should give a possitive value of _end_time" << '\n';
        return;
    }
    end_time = _end_time;
    event *e;
    e = event::get_next_event ();
    while ( e != nullptr && e->trigger_time <= end_time ) {
        if ( cur_time <= e->trigger_time ) {
            cur_time = e->trigger_time;
        }
        else {
            cerr << "cur_time = " << cur_time << ", event trigger_time = " << e->trigger_time << '\n';
            break;
        }

        // cout << "event trigger_time = " << e->trigger_time << '\n';
        e->print(); // for log
        // cout << " event begin" << '\n';
        e->trigger();
        // cout << " event end" << '\n';
        delete e;
        e = event::get_next_event ();
    }
    // cout << "no more event" << '\n';
}

bool mycomp::operator() (const event* lhs, const event* rhs) const {
    // cout << lhs->get_trigger_time() << ", " << rhs->get_trigger_time() << '\n';
    // cout << lhs->type() << ", " << rhs->type() << '\n';
    unsigned int lhs_pri = lhs->event_priority();
    unsigned int rhs_pri = rhs->event_priority();
    // cout << "lhs hash = " << lhs_pri << '\n';
    // cout << "rhs hash = " << rhs_pri << '\n';

    if (reverse) {
        return ((lhs->get_trigger_time()) == (rhs->get_trigger_time())) ? (lhs_pri < rhs_pri): ((lhs->get_trigger_time()) < (rhs->get_trigger_time()));
    }
    else {
        return ((lhs->get_trigger_time()) == (rhs->get_trigger_time())) ? (lhs_pri > rhs_pri): ((lhs->get_trigger_time()) > (rhs->get_trigger_time()));
    }
}

class recv_event: public event {
    public:
        class recv_data; // forward declaration

    private:
        recv_event(recv_event&) {} // this constructor cannot be used
        recv_event() = default; // we don't allow users to new a recv_event by themselv
        unsigned int senderID; // the sender
        unsigned int receiverID; // the receiver; the packet will be given to the receiver
        packet *pkt; // the packet

    protected:
        // this constructor cannot be directly called by users; only by generator
        recv_event(unsigned int _trigger_time, void *data): event(_trigger_time), senderID(BROCAST_ID), receiverID(BROCAST_ID), pkt(nullptr){
            recv_data * data_ptr = (recv_data*) data;
            senderID = data_ptr->s_id;
            receiverID = data_ptr->r_id; // the packet will be given to the receiver
            pkt = data_ptr->_pkt;
        }

    public:
        ~recv_event() override = default;
        // recv_event will trigger the recv function
        void trigger() override;

        unsigned int event_priority() const override;

        // recv_event is derived from event_generator to generate a event
        class recv_event_generator : public event_generator{
                static recv_event_generator sample;
                // this constructor is only for sample to register this event type
                recv_event_generator() { /*cout << "recv_event registered" << '\n';*/ register_event_type(&sample); }
            protected:
                event * generate(unsigned int _trigger_time, void *data) override {
                    // cout << "recv_event generated" << '\n';
                    return new recv_event(_trigger_time, data);
                }

            public:
                string type() override { return "recv_event";}
                ~recv_event_generator() override = default;
        };
        // this class is used to initialize the recv_event
        class recv_data{
            public:
                unsigned int s_id;
                unsigned int r_id;
                packet *_pkt;
        };

        void print () const override;
};
recv_event::recv_event_generator recv_event::recv_event_generator::sample;

void recv_event::trigger() {
    if (pkt == nullptr) {
        cerr << "recv_event error: no pkt!" << '\n';
        return ;
    }
    else if (node::id_to_node(receiverID) == nullptr){
        cerr << "recv_event error: no node " << receiverID << "!" << '\n';
        delete pkt; return ;
    }
    node::id_to_node(receiverID)->recv(pkt);
}
unsigned int recv_event::event_priority() const {
    string string_for_hash;
    string_for_hash = to_string(get_trigger_time()) + to_string(senderID) + to_string (receiverID) + to_string (pkt->get_packet_ID());
    return get_hash_value(string_for_hash);
}
// the recv_event::print() function is used for log file
void recv_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime()
         << "   recID"       << setw(11) << receiverID
         << "   pktID"       << setw(11) << pkt->get_packet_ID()
         << "   srcID"       << setw(11) << pkt->get_header()->get_src_ID()
         << "   dstID"       << setw(11) << pkt->get_header()->get_dst_ID()
         << "   preID"       << setw(11) << pkt->get_header()->get_pre_ID()
         << "   nexID"       << setw(11) << pkt->get_header()->get_nex_ID()
         << "   "            << pkt->type()
         << pkt->addition_information();
//  if ( pkt->type() == "IoT_ctrl_packet" ) cout << "   " << ((IoT_ctrl_payload*)pkt->get_payload())->getCounter();
         cout << '\n';
    // cout << pkt->type()
    //      << "   time "       << setw(11) << event::getCurTime()
    //      << "   recID "      << setw(11) << receiverID
    //      << "   pktID"       << setw(11) << pkt->get_packet_ID()
    //      << "   srcID "      << setw(11) << pkt->get_header()->get_src_ID()
    //      << "   dstID"       << setw(11) << pkt->get_header()->get_dst_ID()
    //      << "   preID"       << setw(11) << pkt->get_header()->get_pre_ID()
    //      << "   nexID"       << setw(11) << pkt->get_header()->get_nex_ID()
    //      << '\n';
}

class send_event: public event {
    public:
        class send_data; // forward declaration

    private:
        send_event (send_event &){}
        send_event () = default; // we don't allow users to new a recv_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int senderID; // the sender
        unsigned int receiverID; // the receiver
        packet *pkt; // the packet

    protected:
        send_event (unsigned int _trigger_time, void *data): event(_trigger_time), senderID(BROCAST_ID), receiverID(BROCAST_ID), pkt(nullptr){
            send_data * data_ptr = (send_data*) data;
            senderID = data_ptr->s_id;
            receiverID = data_ptr->r_id;
            pkt = data_ptr->_pkt;
        }

    public:
        ~send_event() override = default;
        // send_event will trigger the send function
        void trigger() override;

        unsigned int event_priority() const override;

        // send_event is derived from event_generator to generate a event
        class send_event_generator : public event_generator{
                static send_event_generator sample;
                // this constructor is only for sample to register this event type
                send_event_generator() { /*cout << "send_event registered" << '\n';*/ register_event_type(&sample); }
            protected:
                event * generate(unsigned int _trigger_time, void *data) override {
                    // cout << "send_event generated" << '\n';
                    return new send_event(_trigger_time, data);
                }

            public:
                string type() override { return "send_event";}
                ~send_event_generator() override = default;
        };
        // this class is used to initialize the send_event
        class send_data{
            public:
                unsigned int s_id;
                unsigned int r_id;
                packet *_pkt;
                unsigned int t;
        };

        void print () const override;
};
send_event::send_event_generator send_event::send_event_generator::sample;

void send_event::trigger() {
    if (pkt == nullptr) {
        cerr << "send_event error: no pkt!" << '\n';
        return ;
    }
    else if (node::id_to_node(senderID) == nullptr){
        cerr << "send_event error: no node " << senderID << "!" << '\n';
        delete pkt; return ;
    }
    node::id_to_node(senderID)->send(pkt);
}
unsigned int send_event::event_priority() const {
    string string_for_hash;
    string_for_hash = to_string(get_trigger_time()) + to_string(senderID) + to_string (receiverID) + to_string (pkt->get_packet_ID());
    return get_hash_value(string_for_hash);
}
// the send_event::print() function is used for log file
void send_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime()
         << "   senID"       << setw(11) << senderID
         << "   pktID"       << setw(11) << pkt->get_packet_ID()
         << "   srcID"       << setw(11) << pkt->get_header()->get_src_ID()
         << "   dstID"       << setw(11) << pkt->get_header()->get_dst_ID()
         << "   preID"       << setw(11) << pkt->get_header()->get_pre_ID()
         << "   nexID"       << setw(11) << pkt->get_header()->get_nex_ID()
         << "   "            << pkt->type()
         << pkt->addition_information()
         //<< "   msg"         << setw(11) << dynamic_cast<IoT_data_payload*>(pkt->get_payload())->getMsg()
         << '\n';
    // cout << pkt->type()
    //      << "   time "       << setw(11) << event::getCurTime()
    //      << "   senID "      << setw(11) << senderID
    //      << "   pktID"       << setw(11) << pkt->get_packet_ID()
    //      << "   srcID "      << setw(11) << pkt->get_header()->get_src_ID()
    //      << "   dstID"       << setw(11) << pkt->get_header()->get_dst_ID()
    //      << "   preID"       << setw(11) << pkt->get_header()->get_pre_ID()
    //      << "   nexID"       << setw(11) << pkt->get_header()->get_nex_ID()
    //      << '\n';
}

////////////////////////////////////////////////////////////////////////////////

class IoT_data_pkt_gen_event: public event {
    public:
        class gen_data; // forward declaration

    private:
        IoT_data_pkt_gen_event (IoT_data_pkt_gen_event &){}
        IoT_data_pkt_gen_event () = default; // we don't allow users to new a IoT_data_pkt_gen_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst
        // packet *pkt; // the packet
        string msg;

    protected:
        IoT_data_pkt_gen_event (unsigned int _trigger_time, void *data): event(_trigger_time), src(BROCAST_ID), dst(BROCAST_ID){
            pkt_gen_data * data_ptr = (pkt_gen_data*) data;
            src = data_ptr->src_id;
            dst = data_ptr->dst_id;
            // pkt = data_ptr->_pkt;
            msg = data_ptr->msg;
        }

    public:
        ~IoT_data_pkt_gen_event() override = default;
        // IoT_data_pkt_gen_event will trigger the packet gen function
        void trigger() override;

        unsigned int event_priority() const override;

        // IoT_data_pkt_gen_event_generator is derived from event_generator to generate an event
        class IoT_data_pkt_gen_event_generator : public event_generator{
                static IoT_data_pkt_gen_event_generator sample;
                // this constructor is only for sample to register this event type
                IoT_data_pkt_gen_event_generator() { /*cout << "send_event registered" << '\n';*/ register_event_type(&sample); }
            protected:
                event * generate(unsigned int _trigger_time, void *data) override {
                    // cout << "send_event generated" << '\n';
                    return new IoT_data_pkt_gen_event(_trigger_time, data);
                }

            public:
                string type() override { return "IoT_data_pkt_gen_event";}
                ~IoT_data_pkt_gen_event_generator() override = default;
        };
        // this class is used to initialize the IoT_data_pkt_gen_event
        class pkt_gen_data{
            public:
                unsigned int src_id;
                unsigned int dst_id;
                string msg;
                // packet *_pkt;
        };

        void print () const override;
};
IoT_data_pkt_gen_event::IoT_data_pkt_gen_event_generator IoT_data_pkt_gen_event::IoT_data_pkt_gen_event_generator::sample;

void IoT_data_pkt_gen_event::trigger() {
    if (node::id_to_node(src) == nullptr){
        cerr << "IoT_data_pkt_gen_event error: no node " << src << "!" << '\n';
        return ;
    }
    else if ( dst != BROCAST_ID && node::id_to_node(dst) == nullptr ) {
        cerr << "IoT_data_pkt_gen_event error: no node " << dst << "!" << '\n';
        return;
    }

    IoT_data_packet *pkt = dynamic_cast<IoT_data_packet*> ( packet::packet_generator::generate("IoT_data_packet") );
    if (pkt == nullptr) {
        cerr << "packet type is incorrect" << '\n'; return;
    }
    IoT_data_header *hdr = dynamic_cast<IoT_data_header*> ( pkt->get_header() );
    IoT_data_payload *pld = dynamic_cast<IoT_data_payload*> ( pkt->get_payload() );

    if (hdr == nullptr) {
        cerr << "header type is incorrect" << '\n'; return ;
    }
    if (pld == nullptr) {
        cerr << "payload type is incorrect" << '\n'; return ;
    }

    hdr->set_src_ID(src);
    hdr->set_dst_ID(dst);
    hdr->set_pre_ID(src); // this column is not important when the packet is first received by the src (i.e., just generated)
    hdr->set_nex_ID(src); // this column is not important when the packet is first received by the src (i.e., just generated)

    pld->set_msg(msg);

    recv_event::recv_data e_data;
    e_data.s_id = src;
    e_data.r_id = src; // to make the packet start from the src
    e_data._pkt = pkt;

    recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event", trigger_time, (void *)&e_data) );

}
unsigned int IoT_data_pkt_gen_event::event_priority() const {
    string string_for_hash;
    string_for_hash = to_string(get_trigger_time()) + to_string(src) + to_string (dst) ; //to_string (pkt->get_packet_ID());
    return get_hash_value(string_for_hash);
}
// the IoT_data_pkt_gen_event::print() function is used for log file
void IoT_data_pkt_gen_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime()
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
         << "   srcID"       << setw(11) << src
         << "   dstID"       << setw(11) << dst
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
         << "   IoT_data_packet generating"
         << '\n';
}

class IoT_ctrl_pkt_gen_event: public event {
    public:
        class gen_data; // forward declaration

    private:
        IoT_ctrl_pkt_gen_event (IoT_ctrl_pkt_gen_event &){}
        IoT_ctrl_pkt_gen_event () = default; // we don't allow users to new a IoT_ctrl_pkt_gen_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst
        // unsigned int mat;
        // unsigned int act;
        // packet *pkt; // the packet
        string msg;
        // double per; // percentage

    protected:
        IoT_ctrl_pkt_gen_event (unsigned int _trigger_time, void *data): event(_trigger_time), src(BROCAST_ID), dst(BROCAST_ID){
            pkt_gen_data * data_ptr = (pkt_gen_data*) data;
            src = data_ptr->src_id;
            dst = data_ptr->dst_id;
            // pkt = data_ptr->_pkt;
            // mat = data_ptr->mat_id;
            // act = data_ptr->act_id;
            msg = data_ptr->msg;
            // per = data_ptr->per;
        }

    public:
        ~IoT_ctrl_pkt_gen_event() override = default;
        // IoT_ctrl_pkt_gen_event will trigger the packet gen function
        void trigger() override;

        unsigned int event_priority() const override;

        // IoT_ctrl_pkt_gen_event_generator is derived from event_generator to generate an event
        class IoT_ctrl_pkt_gen_event_generator : public event_generator{
                static IoT_ctrl_pkt_gen_event_generator sample;
                // this constructor is only for sample to register this event type
                IoT_ctrl_pkt_gen_event_generator() { /*cout << "send_event registered" << '\n';*/ register_event_type(&sample); }
            protected:
                event * generate(unsigned int _trigger_time, void *data) override {
                    // cout << "send_event generated" << '\n';
                    return new IoT_ctrl_pkt_gen_event(_trigger_time, data);
                }

            public:
                string type() override { return "IoT_ctrl_pkt_gen_event";}
                ~IoT_ctrl_pkt_gen_event_generator() override = default;
        };
        // this class is used to initialize the IoT_ctrl_pkt_gen_event
        class pkt_gen_data{
            public:
                unsigned int src_id; // the sink
                unsigned int dst_id; // the node that should update its rule
                // unsigned int mat_id; // the target of the rule
                // unsigned int act_id; // the next hop toward the target recorded in the rule
                string msg;
                // double per; // the percentage
                // packet *_pkt;
        };

        void print () const override;
};
IoT_ctrl_pkt_gen_event::IoT_ctrl_pkt_gen_event_generator IoT_ctrl_pkt_gen_event::IoT_ctrl_pkt_gen_event_generator::sample;

void IoT_ctrl_pkt_gen_event::trigger() {

    IoT_ctrl_packet *pkt = dynamic_cast<IoT_ctrl_packet*> ( packet::packet_generator::generate("IoT_ctrl_packet") );
    if (pkt == nullptr) {
        cerr << "packet type is incorrect" << '\n'; return;
    }
    IoT_ctrl_header *hdr = dynamic_cast<IoT_ctrl_header*> ( pkt->get_header() );
    IoT_ctrl_payload *pld = dynamic_cast<IoT_ctrl_payload*> ( pkt->get_payload() );

    if (hdr == nullptr) {
        cerr << "header type is incorrect" << '\n'; return ;
    }
    if (pld == nullptr) {
        cerr << "payload type is incorrect" << '\n'; return ;
    }

    hdr->set_src_ID(src);
    hdr->set_dst_ID(dst);
    hdr->set_pre_ID(src);
    hdr->set_nex_ID(src);

    // payload
    pld->set_msg(msg);
    // pld->setMatID(mat);
    // pld->setActID(act);
    // pld->setPer(per);

    recv_event::recv_data e_data;
    e_data.s_id = src;
    e_data.r_id = src;
    e_data._pkt = pkt;

    recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event",trigger_time, (void *)&e_data) );
}
unsigned int IoT_ctrl_pkt_gen_event::event_priority() const {
    string string_for_hash;
    // string_for_hash = to_string(get_trigger_time()) + to_string(src) + to_string(dst) + to_string(mat) + to_string(act); //to_string (pkt->get_packet_ID());
    string_for_hash = to_string(get_trigger_time()) + to_string(src) + to_string(dst) ; //to_string (pkt->get_packet_ID());
    return get_hash_value(string_for_hash);
}
// the IoT_ctrl_pkt_gen_event::print() function is used for log file
void IoT_ctrl_pkt_gen_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime()
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
         << "   srcID"       << setw(11) << src
         << "   dstID"       << setw(11) << dst
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
        //  << "   matID"       << setw(11) << mat
        //  << "   actID"       << setw(11) << act
         << "   IoT_ctrl_packet generating"
         << '\n';
}



class AGG_ctrl_pkt_gen_event: public event {
    public:
        class gen_data; // forward declaration

    private:
        AGG_ctrl_pkt_gen_event (AGG_ctrl_pkt_gen_event &){}
        AGG_ctrl_pkt_gen_event () = default; // we don't allow users to new a AGG_ctrl_pkt_gen_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst
        // unsigned int mat;
        // unsigned int act;
        // packet *pkt; // the packet
        string msg;
        // double per; // percentage

    protected:
        AGG_ctrl_pkt_gen_event (unsigned int _trigger_time, void *data): event(_trigger_time), src(BROCAST_ID), dst(BROCAST_ID){
            pkt_gen_data * data_ptr = (pkt_gen_data*) data;
            src = data_ptr->src_id;
            dst = data_ptr->dst_id;
            // pkt = data_ptr->_pkt;
            // mat = data_ptr->mat_id;
            // act = data_ptr->act_id;
            msg = data_ptr->msg;
            // per = data_ptr->per;
        }

    public:
        ~AGG_ctrl_pkt_gen_event() override = default;
        // AGG_ctrl_pkt_gen_event will trigger the packet gen function
        void trigger() override;

        unsigned int event_priority() const override;

        // AGG_ctrl_pkt_gen_event_generator is derived from event_generator to generate an event
        class AGG_ctrl_pkt_gen_event_generator : public event_generator{
                static AGG_ctrl_pkt_gen_event_generator sample;
                // this constructor is only for sample to register this event type
                AGG_ctrl_pkt_gen_event_generator() { /*cout << "send_event registered" << '\n';*/ register_event_type(&sample); }
            protected:
                event * generate(unsigned int _trigger_time, void *data) override {
                    // cout << "send_event generated" << '\n';
                    return new AGG_ctrl_pkt_gen_event(_trigger_time, data);
                }

            public:
                string type() override { return "AGG_ctrl_pkt_gen_event";}
                ~AGG_ctrl_pkt_gen_event_generator() override = default;
        };
        // this class is used to initialize the AGG_ctrl_pkt_gen_event
        class pkt_gen_data{
            public:
                unsigned int src_id; // the sink
                unsigned int dst_id; // the node that should update its rule
                // unsigned int mat_id; // the target of the rule
                // unsigned int act_id; // the next hop toward the target recorded in the rule
                string msg;
                // double per; // the percentage
                // packet *_pkt;
        };

        void print () const override;
};
AGG_ctrl_pkt_gen_event::AGG_ctrl_pkt_gen_event_generator AGG_ctrl_pkt_gen_event::AGG_ctrl_pkt_gen_event_generator::sample;

void AGG_ctrl_pkt_gen_event::trigger() {

    AGG_ctrl_packet *pkt = dynamic_cast<AGG_ctrl_packet*> ( packet::packet_generator::generate("AGG_ctrl_packet") );
    if (pkt == nullptr) {
        cerr << "packet type is incorrect" << '\n'; return;
    }
    AGG_ctrl_header *hdr = dynamic_cast<AGG_ctrl_header*> ( pkt->get_header() );
    AGG_ctrl_payload *pld = dynamic_cast<AGG_ctrl_payload*> ( pkt->get_payload() );

    if (hdr == nullptr) {
        cerr << "header type is incorrect" << '\n'; return ;
    }
    if (pld == nullptr) {
        cerr << "payload type is incorrect" << '\n'; return ;
    }

    hdr->set_src_ID(src);
    hdr->set_dst_ID(dst);
    hdr->set_pre_ID(src);
    hdr->set_nex_ID(src);

    // payload
    pld->set_msg(msg);
    // pld->setMatID(mat);
    // pld->setActID(act);
    // pld->setPer(per);

    recv_event::recv_data e_data;
    e_data.s_id = src;
    e_data.r_id = src;
    e_data._pkt = pkt;

    recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event",trigger_time, (void *)&e_data) );
}
unsigned int AGG_ctrl_pkt_gen_event::event_priority() const {
    string string_for_hash;
    // string_for_hash = to_string(get_trigger_time()) + to_string(src) + to_string(dst) + to_string(mat) + to_string(act); //to_string (pkt->get_packet_ID());
    string_for_hash = to_string(get_trigger_time()) + to_string(src) + to_string(dst) ; //to_string (pkt->get_packet_ID());
    return get_hash_value(string_for_hash);
}
// the AGG_ctrl_pkt_gen_event::print() function is used for log file
void AGG_ctrl_pkt_gen_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime()
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
         << "   srcID"       << setw(11) << src
         << "   dstID"       << setw(11) << dst
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
        //  << "   matID"       << setw(11) << mat
        //  << "   actID"       << setw(11) << act
         << "   AGG_ctrl_packet generating"
         << '\n';
}


class DIS_ctrl_pkt_gen_event: public event {
    public:
        class gen_data; // forward declaration

    private:
        DIS_ctrl_pkt_gen_event (DIS_ctrl_pkt_gen_event &){}
        DIS_ctrl_pkt_gen_event () = default; // we don't allow users to new a DIS_ctrl_pkt_gen_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst
        // unsigned int mat;
        // unsigned int act;
        // packet *pkt; // the packet
        string msg;
        // double per; // percentage
        int parent;

    protected:
        DIS_ctrl_pkt_gen_event (unsigned int _trigger_time, void *data): event(_trigger_time), src(BROCAST_ID), dst(BROCAST_ID){
            pkt_gen_data * data_ptr = (pkt_gen_data*) data;
            src = data_ptr->src_id;
            dst = data_ptr->dst_id;
            parent = data_ptr->parent;
            // pkt = data_ptr->_pkt;
            // mat = data_ptr->mat_id;
            // act = data_ptr->act_id;
            msg = data_ptr->msg;
            // per = data_ptr->per;
        }

    public:
        ~DIS_ctrl_pkt_gen_event() override = default;
        // DIS_ctrl_pkt_gen_event will trigger the packet gen function
        void trigger() override;

        unsigned int event_priority() const override;

        // DIS_ctrl_pkt_gen_event_generator is derived from event_generator to generate an event
        class DIS_ctrl_pkt_gen_event_generator : public event_generator{
                static DIS_ctrl_pkt_gen_event_generator sample;
                // this constructor is only for sample to register this event type
                DIS_ctrl_pkt_gen_event_generator() { /*cout << "send_event registered" << '\n';*/ register_event_type(&sample); }
            protected:
                event * generate(unsigned int _trigger_time, void *data) override {
                    // cout << "send_event generated" << '\n';
                    return new DIS_ctrl_pkt_gen_event(_trigger_time, data);
                }

            public:
                string type() override { return "DIS_ctrl_pkt_gen_event";}
                ~DIS_ctrl_pkt_gen_event_generator() override = default;
        };
        // this class is used to initialize the DIS_ctrl_pkt_gen_event
        class pkt_gen_data{
            public:
                unsigned int src_id; // the sink
                unsigned int dst_id; // the node that should update its rule
                // unsigned int mat_id; // the target of the rule
                // unsigned int act_id; // the next hop toward the target recorded in the rule
                unsigned int parent;
                string msg;
                // double per; // the percentage
                // packet *_pkt;
        };

        void print () const override;
};
DIS_ctrl_pkt_gen_event::DIS_ctrl_pkt_gen_event_generator DIS_ctrl_pkt_gen_event::DIS_ctrl_pkt_gen_event_generator::sample;

void DIS_ctrl_pkt_gen_event::trigger() {

    DIS_ctrl_packet *pkt = dynamic_cast<DIS_ctrl_packet*> ( packet::packet_generator::generate("DIS_ctrl_packet") );
    if (pkt == nullptr) {
        cerr << "packet type is incorrect" << '\n'; return;
    }
    DIS_ctrl_header *hdr = dynamic_cast<DIS_ctrl_header*> ( pkt->get_header() );
    DIS_ctrl_payload *pld = dynamic_cast<DIS_ctrl_payload*> ( pkt->get_payload() );

    if (hdr == nullptr) {
        cerr << "header type is incorrect" << '\n'; return ;
    }
    if (pld == nullptr) {
        cerr << "payload type is incorrect" << '\n'; return ;
    }

    hdr->set_src_ID(src);
    hdr->set_dst_ID(dst);
    hdr->set_pre_ID(src);
    hdr->set_nex_ID(src);

    // payload
    pld->set_parent(parent);
    pld->set_msg(msg);
    // pld->setMatID(mat);
    // pld->setActID(act);
    // pld->setPer(per);

    recv_event::recv_data e_data;
    e_data.s_id = src;
    e_data.r_id = src;
    e_data._pkt = pkt;

    recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event",trigger_time, (void *)&e_data) );
}
unsigned int DIS_ctrl_pkt_gen_event::event_priority() const {
    string string_for_hash;
    // string_for_hash = to_string(get_trigger_time()) + to_string(src) + to_string(dst) + to_string(mat) + to_string(act); //to_string (pkt->get_packet_ID());
    string_for_hash = to_string(get_trigger_time()) + to_string(src) + to_string(dst) ; //to_string (pkt->get_packet_ID());
    return get_hash_value(string_for_hash);
}
// the DIS_ctrl_pkt_gen_event::print() function is used for log file
void DIS_ctrl_pkt_gen_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime()
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
         << "   srcID"       << setw(11) << src
         << "   dstID"       << setw(11) << dst
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
        //  << "   matID"       << setw(11) << mat
        //  << "   actID"       << setw(11) << act
         << "   DIS_ctrl_packet generating"
         << '\n';
}

////////////////////////////////////////////////////////////////////////////////

class link {
        // all links created in the program
        static map< pair<unsigned int,unsigned int>, link*> id_id_link_table;

        unsigned int id1; // from
        unsigned int id2; // to

    protected:
        link(link&){} // this constructor should not be used
        link() = default; // this constructor should not be used
        link(unsigned int _id1, unsigned int _id2): id1(_id1), id2(_id2) { id_id_link_table[pair<unsigned int,unsigned int>(id1,id2)] = this; }

    public:
        virtual ~link() {
            id_id_link_table.erase (pair<unsigned int,unsigned int>(id1,id2)); // erase the link
        }

        static link * id_id_to_link (unsigned int _id1, unsigned int _id2) {
            return ((id_id_link_table.find(pair<unsigned int,unsigned int>(_id1,_id2))!=id_id_link_table.end()) ? id_id_link_table[pair<unsigned,unsigned>(_id1,_id2)]: nullptr) ;
        }

        virtual double getLatency() = 0; // you must implement your own latency

        static void del_link (unsigned int _id1, unsigned int _id2) {
            pair<unsigned int, unsigned int> temp;
            if (id_id_link_table.find(temp)!=id_id_link_table.end()) {
                id_id_link_table.erase(temp);
            }
        }

        static unsigned int get_link_num () { return id_id_link_table.size(); }

        class link_generator {
                // lock the copy constructor
                link_generator(link_generator &) = default;
                // store all possible types of link
                static map<string,link_generator*> prototypes;
            protected:
                // allow derived class to use it
                link_generator() = default;
                // after you create a new link type, please register the factory of this link type by this function
                void register_link_type(link_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your link
                virtual link* generate(unsigned int _id1, unsigned int _id2) = 0;
            public:
                // you have to implement your own type() to return your link type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of link derived
        	    static link * generate (const string &type, unsigned int _id1, unsigned int _id2) {
        	        if(id_id_link_table.find(pair<unsigned int,unsigned int>(_id1,_id2))!=id_id_link_table.end()){
        	            std::cerr << "duplicate link id" << '\n'; // link id is duplicated
        	            return nullptr;
        	        }
        	        else if ( BROCAST_ID == _id1 || BROCAST_ID == _id2 ) {
        	            cerr << "BROCAST_ID cannot be used" << '\n';
        	            return nullptr;
        	        }
            		else if (prototypes.find(type) != prototypes.end()){ // if this type derived exists
            		    link * created_link = prototypes[type]->generate(_id1,_id2);
            			return created_link; // generate it!!
            		}
            		std::cerr << "no such link type" << '\n'; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered link types: " << '\n';
            	    for (map<string,link::link_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++) {
            	        cout << it->second->type() << '\n';
                    }
            	}
            	virtual ~link_generator() = default;
        };
};
map<string,link::link_generator*> link::link_generator::prototypes;
map<pair<unsigned int,unsigned int>, link*> link::id_id_link_table;

void node::add_phy_neighbor (unsigned int _id, const string &link_type){
    if (id == _id) {return;} // if the two nodes are the same...
    if (id_node_table.find(_id)==id_node_table.end()) {return;} // if this node does not exist
    if (phy_neighbors.find(_id)!=phy_neighbors.end()) {return;} // if this neighbor has been added
    phy_neighbors[_id] = true;

    link::link_generator::generate(link_type,id,_id);
}
void node::del_phy_neighbor (unsigned int _id){
    phy_neighbors.erase(_id);

}


class simple_link: public link {
    protected:
        simple_link() = default; // it should not be used outside the class
        simple_link(simple_link&) {} // it should not be used
        simple_link(unsigned int _id1, unsigned int _id2): link (_id1,_id2){} // this constructor cannot be directly called by users

    public:
        ~simple_link() override = default;
        double getLatency() override { return ONE_HOP_DELAY; } // you can implement your own latency

        // simple_link is derived from link_generator to generate a link
        class simple_link_generator : public link_generator {
                static simple_link_generator sample;
                // this constructor is only for sample to register this link type
                simple_link_generator() { /*cout << "simple_link registered" << '\n';*/ register_link_type(&sample); }
            protected:
                link * generate(unsigned int _id1, unsigned int _id2) override
                { /*cout << "simple_link generated" << '\n';*/ return new simple_link(_id1,_id2); }
            public:
                string type() override { return "simple_link"; }
                ~simple_link_generator() override = default;
        };
};

simple_link::simple_link_generator simple_link::simple_link_generator::sample;


// the IoT_data_packet_event function is used to add an initial event
void IoT_data_packet_event (unsigned int src, unsigned int dst=0, unsigned int t = 0, string msg = "default"){
    if ( node::id_to_node(src) == nullptr || (dst != BROCAST_ID && node::id_to_node(dst) == nullptr) ) {
        cerr << "src or dst is incorrect" << '\n'; return ;
        return;
    }

    IoT_data_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = dst;
    e_data.msg = msg;

    // recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event",t, (void *)&e_data) );
    IoT_data_pkt_gen_event *e = dynamic_cast<IoT_data_pkt_gen_event*> ( event::event_generator::generate("IoT_data_pkt_gen_event",t, (void *)&e_data) );
    if (e == nullptr) {
        cerr << "event type is incorrect" << '\n';
    }
}

// the IoT_ctrl_packet_event function is used to add an initial event

void IoT_ctrl_packet_event (unsigned int src, unsigned int t = event::getCurTime(),
                        string msg = "default") {
        // 1st parameter: the source; the destination that want to broadcast a msg with counter 0 (i.e., match ID)
        // 2nd parameter: time (optional)
        // 3rd parameter: msg (optional)
    if ( node::id_to_node(src) == nullptr ) {
        cerr << "id is incorrect" << '\n'; return;
    }

    // unsigned int src = con_id;
    IoT_ctrl_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = BROCAST_ID;
    // e_data.mat_id = mat;
    // e_data.act_id = act;
    e_data.msg = msg;
    // e_data.per = per;

    IoT_ctrl_pkt_gen_event *e = dynamic_cast<IoT_ctrl_pkt_gen_event*> ( event::event_generator::generate("IoT_ctrl_pkt_gen_event",t, (void *)&e_data) );
    if (e == nullptr) {
        cerr << "event type is incorrect" << '\n';
    }
}

// the AGG_ctrl_packet_event function is used to add an initial event
void AGG_ctrl_packet_event (unsigned int src, unsigned int dst=0, unsigned int t = event::getCurTime(), string msg = "default"){
    if ( node::id_to_node(src) == nullptr || (dst != BROCAST_ID && node::id_to_node(dst) == nullptr) ) {
        cerr << "src or dst is incorrect" << '\n'; return ;
        return;
    }

    AGG_ctrl_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = dst;
    e_data.msg = msg;

    // recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event",t, (void *)&e_data) );
    AGG_ctrl_pkt_gen_event *e = dynamic_cast<AGG_ctrl_pkt_gen_event*> ( event::event_generator::generate("AGG_ctrl_pkt_gen_event",t, (void *)&e_data) );
    if (e == nullptr) {
        cerr << "event type is incorrect" << '\n';
    }
}

// the DIS_ctrl_packet_event function is used to add an initial event
void DIS_ctrl_packet_event (unsigned int sink_id, unsigned int id, unsigned int parent,
                            unsigned int t = event::getCurTime(), string msg = "default"){
    if ( node::id_to_node(sink_id) == nullptr || (id != BROCAST_ID && node::id_to_node(id) == nullptr) ) {
        cerr << "sink_id or id is incorrect" << '\n'; return ;
        return;
    }

    DIS_ctrl_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = sink_id;
    e_data.dst_id = id;
    e_data.parent = parent;
    e_data.msg = msg;

    // recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event",t, (void *)&e_data) );
    DIS_ctrl_pkt_gen_event *e = dynamic_cast<DIS_ctrl_pkt_gen_event*> ( event::event_generator::generate("DIS_ctrl_pkt_gen_event",t, (void *)&e_data) );
    if (e == nullptr) {
        cerr << "event type is incorrect" << '\n';
    }
}

// send_handler function is used to transmit packet p based on the information in the header
// Note that the packet p will not be discard after send_handler ()

void node::send_handler(packet *p){
    packet *_p = packet::packet_generator::replicate(p);
    send_event::send_data e_data;
    e_data.s_id = _p->get_header()->get_pre_ID();
    e_data.r_id = _p->get_header()->get_nex_ID();
    e_data._pkt = _p;
    send_event *e = dynamic_cast<send_event*> (event::event_generator::generate("send_event",event::getCurTime(), (void *)&e_data) );
    if (e == nullptr) {
        cerr << "event type is incorrect" << '\n';
    }
}

void node::send(packet *p){ // this function is called by event; not for the user
    if (p == nullptr) {return;}

    unsigned int _nexID = p->get_header()->get_nex_ID();
    for ( map<unsigned int,bool>::iterator it = phy_neighbors.begin(); it != phy_neighbors.end(); it ++) {
        unsigned int nb_id = it->first; // neighbor id

        if (nb_id != _nexID && BROCAST_ID != _nexID) {continue;} // this neighbor will not receive the packet

        unsigned int trigger_time = event::getCurTime() + link::id_id_to_link(id, nb_id)->getLatency() ; // we simply assume that the delay is fixed
        // cout << "node " << id << " send to node " <<  nb_id << '\n';
        recv_event::recv_data e_data;
        e_data.s_id = id;    // set the sender   (i.e., preID)
        e_data.r_id = nb_id; // set the receiver (i.e., nexID)

        packet *p2 = packet::packet_generator::replicate(p);
        e_data._pkt = p2;

        recv_event *e = dynamic_cast<recv_event*> (event::event_generator::generate("recv_event", trigger_time, (void*) &e_data)); // send the packet to the neighbor
        if (e == nullptr) {
            cerr << "event type is incorrect" << '\n';
        }
    }
    packet::discard(p);
}

// you have to write the code in recv_handler of IoT_device
void IoT_device::recv_handler (packet *p){
    // in this function, you are "not" allowed to use node::id_to_node(id) !!!!!!!!

    // this is a simple example
    // node 0 broadcasts its message to every node and every node relays the packet "only once" and increases its counter
    // the variable hi is used to examine whether the packet has been received by this node before
    // you can remove the variable hi and create your own routing table in class IoT_device
    if (p == nullptr) {return ;}

    if (p->type() == "IoT_ctrl_packet" && !hi ) { // the device receives a packet from the sink
        IoT_ctrl_packet *p3 = nullptr;
        p3 = dynamic_cast<IoT_ctrl_packet*> (p);
        IoT_ctrl_payload *l3 = nullptr;
        l3 = dynamic_cast<IoT_ctrl_payload*> (p3->get_payload());

        p3->get_header()->set_pre_ID ( getNodeID() );
        p3->get_header()->set_nex_ID ( BROCAST_ID );
        p3->get_header()->set_dst_ID ( BROCAST_ID );

        l3->increase();
        hi = true;
        send_handler(p3);
        // unsigned mat = l3->getMatID();
        // unsigned act = l3->getActID();
        // string msg = l3->getMsg(); // get the msg
    }
    else if (p->type() == "IoT_data_packet" ) { // the device receives a packet
        // cout << "node " << getNodeID() << " send the packet" << '\n';
    }
    else if (p->type() == "AGG_ctrl_packet") {
        AGG_ctrl_packet *p3 = nullptr;
        p3 = dynamic_cast<AGG_ctrl_packet*> (p);
        AGG_ctrl_payload *l3 = nullptr;
        l3 = dynamic_cast<AGG_ctrl_payload*> (p3->get_payload());

        // cout << "node id = " << getNodeID() << ", msg = "  << l3->getMsg() << '\n';
    }
    else if (p->type() == "DIS_ctrl_packet") {
        DIS_ctrl_packet *p3 = nullptr;
        p3 = dynamic_cast<DIS_ctrl_packet*> (p);
        DIS_ctrl_payload *l3 = nullptr;
        l3 = dynamic_cast<DIS_ctrl_payload*> (p3->get_payload());

        // cout << "node id = " << getNodeID() << ", parent = "  << l3->get_parent() << '\n';
    }

    // you should implement the OSPF algorithm in recv_handler
    // getNodeID() returns the id of the current node

    // The current node's neighbors are already stored in the following variable
    // map<unsigned int,bool> node::phy_neighbors
    // however, this variable is private in the class node
    // You have to use node::getPhyNeighbors to get the variable
    // for example, if you want to print all the neighbors of this node
    // const map<unsigned int,bool> &nblist = getPhyNeighbors();
    // cout << "node " << getNodeID() << "'s nblist: ";
    // for (map<unsigned int,bool>::const_iterator it = nblist.begin(); it != nblist.end(); it ++) {
    //     cout << it->first << ", " ;
    // }
    // cout << '\n';


    // you can use p->get_header()->set_src_ID() or get_src_ID()
    //             p->get_header()->set_dst_ID() or get_dst_ID()
    //             p->get_header()->set_pre_ID() or get_pre_ID()
    //             p->get_header()->set_nex_ID() or get_nex_ID() to change or read the packet header

    // In addition, you can get the packet, header, and payload with the correct type
    // in fact, this is downcasting
    // IoT_data_packet * pkt = dynamic_cast<IoT_data_packet*> (p);
    // IoT_data_header * hdr = dynamic_cast<IoT_data_header*> (p->get_header());
    // IoT_data_payload * pld = dynamic_cast<IoT_data_payload*> (p->get_payload());

    // you can also change the IoT_data_payload setting
    // pld->set_msg(string): to set the message transmitted to the destination

    // Besides, you can use packet::packet_generator::generate() to generate a new packet; note that you should fill the header and payload in the packet
    // moreover, you can use "packet *p2 = packet::packet_generator::replicate(p)" to make a clone p2 of packet p
    // note that if the packet is generated or replicated manually, you must delete it by packet::discard() manually before recv_handler finishes

    // "IMPORTANT":
    // You have to "carefully" fill the correct information (e.g., srcID, dstID, ...) in the packet before you send it
    // Note that if you want to transmit a packet to only one next node (i.e., unicast), then you fill the ID of a specific node to "nexID" in the header
    // Otherwise, i.e., you want to broadcasts, then you fill "BROCAST_ID" to "nexID" in the header
    // after that, you can use send() to transmit the packet
    // usage: send_handler (p);

    // note that packet p will be discarded (deleted) after recv_handler(); you don't need to manually delete it
}

int main()
{
    // header::header_generator::print(); // print all registered headers
    // payload::payload_generator::print(); // print all registered payloads
    // packet::packet_generator::print(); // print all registered packets
    // node::node_generator::print(); // print all registered nodes
    // event::event_generator::print(); // print all registered events
    // link::link_generator::print(); // print all registered links

    // read the input and generate devices
    for (unsigned int id = 0; id < 5; id ++){

        node::node_generator::generate("IoT_device",id);

    }

    // please generate the sink by yourself


    // set devices' neighbors
    node::id_to_node(0)->add_phy_neighbor(1);
    node::id_to_node(1)->add_phy_neighbor(0);
    node::id_to_node(0)->add_phy_neighbor(2);
    node::id_to_node(2)->add_phy_neighbor(0);
    node::id_to_node(1)->add_phy_neighbor(2);
    node::id_to_node(2)->add_phy_neighbor(1);
    node::id_to_node(1)->add_phy_neighbor(3);
    node::id_to_node(3)->add_phy_neighbor(1);
    node::id_to_node(2)->add_phy_neighbor(4);
    node::id_to_node(4)->add_phy_neighbor(2);


    // node 0 broadcasts a msg with counter 0 at time 100
    IoT_ctrl_packet_event(0, 100);
    // 1st parameter: the source; the destination that want to broadcast a msg with counter 0 (i.e., match ID)
    // 2nd parameter: time (optional)
    // 3rd parameter: msg for debug information (optional)

    // node 4 sends a packet to node 0 at time 200 --> you need to implement routing tables for IoT_devicees
    IoT_data_packet_event(4, 0, 200); // IoT_data_packet
    // 1st parameter: the source node
    // 2nd parameter: the destination node (sink)
    // 3rd parameter: time (optional)
    // 4th parameter: msg for debug (optional)



    AGG_ctrl_packet_event(4, 0, 250);
    // 1st parameter: the source node
    // 2nd parameter: the destination node (sink)
    // 3rd parameter: time (optional)
    // 4th parameter: msg (for storing nb list)

    DIS_ctrl_packet_event(0, 3, 8, 260);
    // 1st parameter: the source node (sink)
    // 2nd parameter: the destination node
    // 3rd parameter: parent
    // 4th parameter: time (optional)
    // 5th parameter: msg for debug (optional)

    // start simulation!!
    event::start_simulate(300);
    // event::flush_events() ;
    // cout << packet::get_live_packet_num() << '\n';
    return 0;
}

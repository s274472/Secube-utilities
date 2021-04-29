/**
  ******************************************************************************
  * File Name          : SEkey.cpp
  * Description        : SEkey library implementation.
  ******************************************************************************
  *
  * Copyright � 2016-present Blu5 Group <https://www.blu5group.com>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 3 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, see <https://www.gnu.org/licenses/>.
  *
  ******************************************************************************
  */

#include <regex>
#include <fstream>
#include "SEkey.h"
#include "../sefile/environment.h"

using namespace std;

//#define SHARED_WINDOWS_FOLDER /**< This macro must be enable if you plan to use a shared Windows folder to distribute SEkey updates. */
#ifdef _WIN32
string root = "C:\\Users\\matteo\\sekey\\"; /**< Location where update files must be written/read by the admin/users. Replace with the required value (could be set by higher levels, i.e. GUI). */
#elif defined(__linux__) || defined(__APPLE__)
string root = "/home/matteo/Scrivania/SekeyUpdates/";
#endif

sqlite3 *db = nullptr; /**< Global pointer to the SQLite database connection for SEkey. */
bool is_admin = false; /**< Global flag that is true if the SEcube is being used by the administrator and false otherwise. */
bool blocked = false; /**< Global flag that prevents the user to run any SEkey command except the SEkey update, in case the database is not updated to the latest version. */
userinfo currentuser; /**< Global instance that stores the user ID and the username of the user currently logged in. Both fields equal to "admin" for the administrator. */
string logs; /* This string is used to incrementally store the rows of the log file of the user during the update, the string will be flushed to the log file after the commit of the changes. */
string SEcube_root; /**< This is the path of the MicroSD of the SEcube. */
bool SEkey_running = false; // see environment.h
L1 *SEcube = nullptr; // see environment.h

se_key::se_key(std::string& key_id, uint32_t algo, uint32_t key_length, time_t act, time_t exp){
	this->id = key_id;
	this->algorithm = algo;
	this->length = key_length;
	this->activation = act;
	this->expiration = exp;
	/* from here it's just to avoid compiler warnings, this constructor is only used when the key
	 * object requires a comparison with another key object based on the attributes above */
	this->name = "";
	this->owner = "";
	this->status = se_key_status::preactive;
	this->generation = 0;
	this->cryptoperiod = 0;
	this->deactivation = 0;
	this->compromise = 0;
	this->destruction = 0;
	this->suspension = 0;
	this->type = se_key_type::symmetric_data_encryption;
}
se_key::se_key(string& key_id, string& key_name, string& key_owner, se_key_status key_status, uint32_t key_algo,
		uint32_t key_length, time_t gen, time_t act, time_t exp, time_t crypto, time_t deact, se_key_type key_type,
		time_t compr, time_t destr, time_t susp){
	this->id = key_id;
	this->name = key_name;
	this->owner = key_owner;
	this->status = key_status;
	this->algorithm = key_algo;
	this->length = key_length;
	this->generation = gen;
	this->activation = act;
	this->expiration = exp;
	this->cryptoperiod = crypto;
	this->type = key_type;
	this->deactivation = deact;
	this->compromise = compr;
	this->destruction = destr;
	this->suspension = susp;
}
se_key& se_key::operator= (const se_key& key)
{
	if(this != &key){
		this->id = key.id;
		this->name = key.name;
		this->owner = key.owner;
		this->status = key.status;
		this->algorithm = key.algorithm;
		this->length = key.length;
		this->generation = key.generation;
		this->activation = key.activation;
		this->expiration = key.expiration;
		this->cryptoperiod = key.cryptoperiod;
		this->deactivation = key.deactivation;
		this->compromise = key.compromise;
		this->destruction = key.destruction;
		this->suspension = key.suspension;
		this->type = key.type;
	}
	return *this;
}
string& se_key::get_id(){
	return this->id;
}
se_key_status se_key::get_status(){
	return this->status;
}
bool se_key::safer(se_key& chosen){
	if(algocmp(this->algorithm, chosen.algorithm) > 0){
		return true; // choice base on best algorithm
	}
	if(algocmp(this->algorithm, chosen.algorithm) < 0){
		return false;
	}
	if((this->expiration - this->activation) < (chosen.expiration - chosen.activation)){
		return true; // same algorithm, choice based on key usage time (assuming shorter usage time means less probability of attack)
	}
	if((this->expiration - this->activation) > (chosen.expiration - chosen.activation)){
		return false;
	}
	string k1 = this->id.substr(1);
	string k2 = chosen.id.substr(1);
	if(stoul_wrap(k1) < stoul_wrap(k2)){
		return true;
	}
	return false;
}
void se_key::print_key_details(ofstream& sekey_log)
{
	sekey_log << "/************/" << endl;
	sekey_log << "Key ID: " << this->id << endl;
	sekey_log << "Key name: " << this->name << endl;
	sekey_log << "Group owner of the key: " << this->owner << endl;
	sekey_log << "Key status: " << statusmap(this->status) << endl;
	sekey_log << "Key algorithm: " << algomap(this->algorithm) << endl;
	sekey_log << "Key length (bits): " << (this->length)*8 << endl;
	sekey_log << "Key type: " << keytypemap(this->type) << endl;
	sekey_log << "Key cryptoperiod (seconds): " << cryptoperiod_to_days((uint32_t)this->cryptoperiod) << endl;
	sekey_log << "Key generation date: " << epoch_to_localtime(this->generation) << endl;
	sekey_log << "Key activation date: " << epoch_to_localtime(this->activation) << endl;
	sekey_log << "Key suspension date: " << epoch_to_localtime(this->suspension) << endl;
	sekey_log << "Key expiration date: " << epoch_to_localtime(this->expiration) << endl;
	sekey_log << "Key compromise date: " << epoch_to_localtime(this->compromise) << endl;
	sekey_log << "Key deactivation date: " << epoch_to_localtime(this->deactivation) << endl;
	sekey_log << "Key destruction date: " << epoch_to_localtime(this->destruction) << endl;
}

void se_user::set_id(string& new_id)
{
	this->id = new_id;
}
string& se_user::get_id()
{
	return this->id;
}
string& se_user::get_name()
{
	return this->name;
}
void se_user::set_name(string& new_name)
{
	this->name = new_name;
}
void se_user::add_group(string& group)
{
	this->groups.push_back(group);
}
void se_user::print_user_details(ofstream& sekey_log)
{
	sekey_log << "/************/" << endl;
	sekey_log << "User ID: " << this->id << endl;
	sekey_log << "Username: " << this->name << endl;
	sekey_log << "SEcube S/N: " << this->sn << endl;
	sekey_log << "SEcube user (low privilege) PIN: " << this->user_pin << endl;
	sekey_log << "SEcube admin (high privilege) PIN: " << this->admin_pin << endl;
	sekey_log << "Algorithm for updates: " << this->algorithm << endl;
	sekey_log << "Key ID for updates: " << this->k1 << endl;
	sekey_log << "Key ID for encrypted keys inside updates: " << this->k2 << endl;
	if(this->init != 0){
		sekey_log << "User SEcube status: initialized." << endl;
	} else {
		sekey_log << "User SEcube status: not initialized." << endl;
	}
	sekey_log << "Update counter: " << this->update_cnt << endl;
	sekey_log << "List of groups to which the user belongs: " << endl;
	for(string& group : this->groups){
		sekey_log << group << endl;
	}
}
se_user::se_user(string& user_id, string& user_name, string& serialnumber, std::string& userpin, std::string& adminpin, uint32_t k1, uint32_t k2, uint32_t algo, uint32_t init_flag, int64_t cnt){
	this->id = user_id;
	this->name = user_name;
	this->sn = serialnumber;
	this->user_pin = userpin;
	this->admin_pin = adminpin;
	this->k1 = k1;
	this->k2 = k2;
	this->init = init_flag;
	this->update_cnt = cnt;
	if(algo == L1Algorithms::Algorithms::AES_HMACSHA256){
		this->algorithm = "AES_HMAC_SHA256";
	} else {
		this->algorithm = "invalid";
	}
}

se_group::se_group(string& groupid, string& groupname, group_policy gpolicy){
	this->id = groupid;
	this->name = groupname;
	this->users_counter = 0;
	this->keys_counter = 0;
	this->policy.set_algorithm(gpolicy.get_algorithm());
	this->policy.set_default_cryptoperiod(gpolicy.get_default_cryptoperiod());
	this->policy.set_max_keys(gpolicy.get_max_keys());
}
string& se_group::get_id(){
	return this->id;
}
string& se_group::get_name(){
	return this->name;
}
uint32_t se_group::get_keys_algorithm(){
	return this->policy.get_algorithm();
}
uint32_t se_group::get_users_counter(){
	return this->users_counter;
}
uint32_t se_group::get_keys_counter(){
	return this->keys_counter;
}
uint32_t se_group::get_keys_maxnumber(){
	return this->policy.get_max_keys();
}
uint32_t se_group::get_keys_cryptoperiod(){
	return this->policy.get_default_cryptoperiod();
}
void se_group::set_id(string& new_id){
	this->id = new_id;
}
void se_group::set_name(string& new_name){
	this->name = new_name;
}
void se_group::set_users_counter(uint32_t cnt){
	this->users_counter = cnt;
}
void se_group::set_keys_counter(uint32_t cnt){
	this->keys_counter = cnt;
}
void se_group::set_keys_maxnumber(uint32_t max){
	this->policy.set_max_keys(max);
}
void se_group::set_keys_algorithm(uint32_t algo){
	this->policy.set_algorithm(algo);
}
void se_group::set_keys_cryptoperiod(uint32_t cryptoperiod){
	this->policy.set_default_cryptoperiod(cryptoperiod);
}
void se_group::print_group_details(ofstream& sekey_log)
{
	sekey_log << "/************/" << endl;
	sekey_log << "Group ID: " << this->id << endl;
	sekey_log << "Group name: " << this->name << endl;
	sekey_log << "Number of users in this group: " << this->users_counter << endl;
	sekey_log << "Number of keys of this group: " << this->keys_counter << endl;
	sekey_log << "Maximum number of keys for this group: " << this->policy.max_keys << endl;
	sekey_log << "Algorithm used by the keys of this group: " << algomap(this->policy.algorithm) << endl;
	sekey_log << "Validity period of the keys of this group when activated: " << cryptoperiod_to_days(this->policy.default_cryptoperiod) << endl;
}
uint32_t group_policy::get_algorithm(){
	return this->algorithm;
}
void group_policy::set_algorithm(uint32_t algo){
	this->algorithm = algo;
}
uint32_t group_policy::get_default_cryptoperiod(){
	return this->default_cryptoperiod;
}
void group_policy::set_default_cryptoperiod(uint32_t cryptoperiod){
	 this->default_cryptoperiod = cryptoperiod;
}
uint32_t group_policy::get_max_keys(){
	return this->max_keys;
}
void group_policy::set_max_keys(uint32_t max){
	this->max_keys = max;
}
bool group_policy::isvalid(){
	if(!algovalid(this->algorithm) || this->default_cryptoperiod == 0 || this->max_keys == 0){
		return false;
	}
	return true;
}
group_policy::group_policy(uint32_t max, uint32_t algo, uint32_t cryptoperiod){
	this->algorithm = algo;
	this->default_cryptoperiod = cryptoperiod;
	this->max_keys = max;
}

/* GENERAL SEKEY APIs */
int sekey_start(L0& l0, L1 *l1ptr){
	try{
		if(l1ptr == nullptr){ return SEKEY_ERR_PARAMS; }
		SEcube = l1ptr;
		int open_flags = 0, rc;
		string query, dbname, msg, microsd;
		statement sqlstmt;
		if(get_microsd_path(l0, microsd)){
			SEcube = nullptr;
			return SEKEY_ERR;
		}
		SEcube_root = microsd;
		/* check if the SEcube is used as administrator or user */
		if(SEcube->L1GetAccessType() == SE3_ACCESS_ADMIN){
			is_admin = true;
			currentuser.userid = "admin"; // in this case the currentuser info won't be used
			currentuser.username = "admin";
		} else {
			is_admin = false;
			// get user info stored in the device flash
			if(!SEcube->L1SEkey_Info(currentuser.userid, currentuser.username, L1SEkey::Direction::LOAD)){
				throw "generic error"; // throw used simply to jump to catch immediately
			}
		}
		SEcube->GetDeviceSerialNumber(currentuser.device_sn);
		dbname = SEcube_root + currentuser.device_sn + ".sqlite";
		// initialize the security properties of the SEfile object used to manage the database of SEkey
		unique_ptr<SEfile> dbfile = make_unique<SEfile>();
		if(dbfile->secure_init(l1ptr, L1Key::Id::RESERVED_ID_SEKEY_SECUREDB, L1Algorithms::Algorithms::AES_HMACSHA256)){ throw "generic error"; }
		char filename_[MAX_PATHNAME];
		memset(filename_, '\0', MAX_PATHNAME);
		get_filename((char*)dbname.c_str(), filename_);
		memcpy(dbfile->handleptr->name, filename_, strlen(filename_));
		// add a weak pointer to the list of databases used by the SEcureDB functions called by SQLite
		databases.push_back(std::move(dbfile));
		// check if the database file already exists
		if((rc=file_exists(dbname)) == SEKEY_FILE_NOT_FOUND){
			open_flags = SQLITE_OPEN_CREATE;
		} else {
			open_flags = 0; // bitwise OR operator with this value won't change SQLITE_OPEN_READWRITE
		}
		if(rc == SEKEY_ERR){
			throw "generic error";
		}
		/* open the database file enabling also the EXTRA synchronous mode and performing a fake transaction
		 * using a dummy table. this fake transaction will always be rolled back and is used only to process
		 * any journal file left on the disk in case of a previous crash, power loss or fatal error. the
		 * journal file in fact is processed automatically only if it is a hot-journal but it is a
		 * hot-journal only if the crash happened during a commit or during a rollback. in case of a crash
		 * in the middle of a transaction before the commit or rollback, the journal stays there and is not
		 * process automatically when the db is opened. it is processed (rolled back) on the next transaction,
		 * but we need to rollback the journal as soon as possible because we want to revert SEkey immediately
		 * to the latest coherent data. therefore we use this trik with a begin, write on the db and rollback
		 * to get rid of the journal in advance. */
		if(((rc = sqlite3_open_v2(dbname.c_str(), &db, SQLITE_OPEN_READWRITE | open_flags, nullptr)) != SQLITE_OK) ||
		   (sqlite3_exec(db, "PRAGMA synchronous = 2;", nullptr, nullptr, nullptr) != SQLITE_OK)){
			throw "generic error";
		}
		/* warning: keep synchronous = 2 (or 1 but 2 is better), do not use 3 because it is not compatible, do not use 0 because it is not safe. */
		// enable extended result codes
		sqlite3_extended_result_codes(db, 1);
		// disable database extension loading (security measure)
		sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 0, nullptr);
		// set journal mode to default
		sqlite3_exec(db, "PRAGMA journal_mode = DELETE;", nullptr, nullptr, nullptr);
		// check journal mode
		rc = sqlite3_prepare_v2(db, "PRAGMA journal_mode", -1, sqlstmt.getstmtref(), nullptr);
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				throw "generic error";
			}
			msg.assign(sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0));
			if(msg.compare("delete") != 0){
				sekey_stop();
				return SEKEY_ERR;
			}
		}
		// if the database file has just been created, create the tables
		if(open_flags == SQLITE_OPEN_CREATE){
			query.assign("CREATE TABLE Users(user_id TEXT PRIMARY KEY, username TEXT NOT NULL, serial_number TEXT NOT NULL, secube_user_pin TEXT NOT NULL, secube_admin_pin TEXT NOT NULL, "
						"k1 INTEGER DEFAULT 0, k2 INTEGER DEFAULT 0, key_algo INTEGER DEFAULT 0, init_flag INTEGER DEFAULT 0, update_counter INTEGER DEFAULT 0);"
						"CREATE TABLE Groups(group_id TEXT PRIMARY KEY, group_name TEXT NOT NULL, users_counter INTEGER DEFAULT 0, keys_counter INTEGER DEFAULT 0, "
						"max_keys INTEGER DEFAULT 0, algorithm INTEGER DEFAULT 0, keys_liveness INTEGER DEFAULT 0);"
						"CREATE TABLE UserGroup(user_id TEXT NOT NULL, group_id TEXT NOT NULL, PRIMARY KEY(user_id, group_id));"
						"CREATE TABLE SeKeys(key_id TEXT PRIMARY KEY, key_name TEXT NOT NULL, key_owner TEXT NOT NULL, status INTEGER DEFAULT 0, algorithm INTEGER DEFAULT 0, "
						"key_length INTEGER DEFAULT 0, generation INTEGER DEFAULT 0, activation INTEGER DEFAULT 0, expiration INTEGER DEFAULT 0, cryptoperiod INTEGER DEFAULT 0, "
						"deactivation INTEGER DEFAULT 0, type INTEGER DEFAULT 0, compromise INTEGER DEFAULT 0, destruction INTEGER DEFAULT 0, suspension INTEGER DEFAULT 0);"
						"CREATE TABLE Recovery(user_id TEXT NOT NULL, serial_number TEXT NOT NULL, PRIMARY KEY(user_id, serial_number));");
			/* Notice that the recovery table, when SEkey is in user mode, is not used because the user relies in this strategy:
			 * 1) force SEkey to fetch the latest update from the admin before executing any API allowed in user mode
			 * 2-a) if the update is correctly completed there is no need to go on
			 * 2-b) if the update is not completed try again a limited number of times. if finally SEkey is not updated
			 * set the blocked flag to true in order to prevent any operation but SEkey update, then ask to the admin
			 * for a recovery (i.e. giving the SEcube to the administrator or, in an OTA scenario, asking the server
			 * to tell the admin about the recovery need providing user ID and SEcube serial number).
			 * 3) following step 2-b, the user will perform a recovery of its database and will finally reset the blocked
			 * flag to false.
			 * Notice that if the SEcube is disconnected when the blocked flag is true, this does not affect the behavior of SEkey
			 * because each user-mode API will anyway try to update SEkey. So the flag will be set again if the update will fail, if
			 * the update will succeed then the user will be allowed to use SEkey normally. */
			if((rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr)) != SQLITE_OK){
				throw "generic error";
			}
		}
		// in case of pending journal file on restart, restore the database using a "dummy" transaction
		if((sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK) ||
		   (sqlite3_exec(db, "CREATE TABLE mytable(myval INTEGER DEFAULT 0);", nullptr, nullptr, nullptr) != SQLITE_OK) ||
		   (sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr) != SQLITE_OK)){
			throw "generic error";
		}
		// check database integrity
		rc = sqlite3_prepare_v2(db, "PRAGMA integrity_check", -1, sqlstmt.getstmtref(), nullptr);
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				throw "generic error";
			}
			msg.assign(sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0));
			if(msg.compare("ok") != 0){
				sekey_stop();
				return SEKEY_CORRUPTED;
			}
		}
		if(is_admin){ // perform SEcube flash maintenance
			se3_flash_maintenance_routine();
		} else { // the user must update SEkey as soon as SEkey starts running
			blocked = true; // by default the user is immediately blocked unless we are sure the updates are done
			sekey_update_userdata();
		}
		if((rc = sekey_check_expired_keys()) != SEKEY_OK){ throw "generic error"; }
		string msg_ = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", sekey started";
		sekey_printlog(msg_);
		SEkey_running = true;
		return SEKEY_OK;
	} catch(...){ // we don't really care about exceptions...stopping sekey and returning a generic error is enough
		sekey_stop();
		return SEKEY_ERR;
	}
}
int sekey_stop(){
	try{
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", sekey stopped";
		sekey_printlog(msg);
		sqlite3_close(db); // close the connection to the database (notice that this will automatically call the secure_close for the database)
		db = nullptr; // reset pointer to the database
		SEcube = nullptr;
		currentuser.userid = "";
		currentuser.username = "";
		currentuser.device_sn = "";
		is_admin = false;
		SEkey_running = false;
		return SEKEY_OK;
	} catch(...){
		return SEKEY_ERR;
	}
}
void sekey_printlog(string& msg){
	try{
		string filename, filepath, query = "SELECT k1, key_algo FROM Users WHERE user_id = ?1;";
		SEfile logfile;
		statement sqlstmt;
		bool found = false;
		int rc = 1, creation, pos;
		uint32_t key_id, algo;
		filename = currentuser.device_sn;
		filename.append(".log");
		if(is_admin){
			rc = logfile.secure_init(SEcube, L1Key::Id::RESERVED_ID_SEKEY_SECUREDB, L1Algorithms::Algorithms::AES_HMACSHA256);
		} else {
			if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 1, currentuser.userid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
				return;
			}
			for(;;){
				if((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
					break;
				}
				if(rc != SQLITE_ROW) {
					return;
				}
				found = true;
				key_id = get_u32(sqlstmt.getstmt(), 0);
				algo = get_u32(sqlstmt.getstmt(), 1);
			}
			if(found == false){
				return;
			}
			rc = logfile.secure_init(SEcube, key_id, algo);
		}
		if(rc != 0){
			return;
		}
		if(is_admin){
			filepath = SEcube_root + filename;
		} else {
			filepath = root + filename;
		}
		rc = file_exists(filepath);
		if(rc == SEKEY_ERR){
			return;
		}
		if(rc == SEKEY_FILE_NOT_FOUND){
			creation = SEFILE_NEWFILE;
		} else {
			creation = SEFILE_OPEN;
		}
		if((logfile.secure_open((char*)filepath.c_str(), SEFILE_WRITE, creation) != 0) || (logfile.secure_seek(0, &pos, SEFILE_END) != 0)){
			return;
		}
		if(msg.back() != '\n'){
			msg.push_back('\n');
		}
		logfile.secure_write((uint8_t*)msg.c_str(), msg.size());
		logfile.secure_close();
	} catch(...){
		return; // force no throw behavior
	}
}
int sekey_readlog(string* sn, string& output){
	string filename;
	string filepath;
	SEfile logfile(SEcube);
	uint32_t filedim, bytesread;
	int pos;
	unique_ptr<char[]> filecontent;
	if(sn != nullptr){
		filename = sn->append(".log");
	} else {
		filename = currentuser.device_sn + ".log";
	}
	filepath = root + filename;
	if(secure_getfilesize((char*)filepath.c_str(), &filedim, SEcube) != 0){
		return -1;
	}
	filecontent = make_unique<char[]>(filedim);
	memset(filecontent.get(), 0, filedim);
	int rc = logfile.secure_open((char*)filepath.c_str(), SEFILE_READ, SEFILE_OPEN);
	if(rc != 0){ return -1; }
	rc = logfile.secure_seek(0, &pos, SEFILE_BEGIN);
	if(rc != 0){ return -1; }
	if((logfile.secure_read((uint8_t*)filecontent.get(), filedim, &bytesread) != 0) || (bytesread != filedim)){
		return -1;
	}
	output = string(filecontent.get(), filedim);
	logfile.secure_close();
	return SEKEY_OK;
}
int sekey_update_userdata(){
	if(is_admin){ return SEKEY_ERR_AUTH; }
	blocked = true;
	int tries, rc;
	for(tries = 0; tries < TRY_LIMIT; tries++){
		/* start the db transaction. if something goes wrong the db will be rolled back to the previous version. the flash inside the SEcube
		 * then could be not coherent with the content of the database but, when something goes wrong with the update, the SEcube is blocked
		 * from doing any operation other than retrying the update or quitting the update requesting for a complete database recovery to the
		 * administrator. notice that, even if the SEcube flash content may be not coherent with the database, it will be coherent once the
		 * update has been successfully completed or once the recovery has been completed (thanks to the database maintenance routine and to
		 * the fact that, in case we want to store something with an ID which is already in the flash, we overwrite the flash content). */
		try{
			rc = process_update_file();
			if(rc == SEKEY_OK){ // update done
				blocked = false; // now the device can be used safely
				se3_flash_maintenance_routine(); // clean SEcube flash (no throw function)
				string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", user sekey update complete";
				sekey_printlog(msg);
				return SEKEY_OK;
			}
			if((rc == SEKEY_ERR) || (rc == SEKEY_UNCHANGED)){ // update failed
				throw "generic error"; // throw simply to jump to the catch clause (is the same we would write here)
			}
			if(rc == SEKEY_RESTART){
				return rc;
			}
		} catch (...) {
			if(sqlite3_get_autocommit(db) == 0){
				rc = rollback_transaction(); // transaction still active, rollback
				if(rc == SEKEY_RESTART){ // rollback failed
					return rc; // this happens when the rollback fails
					/* returning restart will tell the caller to close the application, reconnect the SEcube
					 * and restart the application, this will restart SEkey (and the automatic rollback
					 * implemented in the SEkey_start API will clean the journal file left on disk) so the
					 * database will be coherent and the device will be able to check again for updates */
				}
				if(rc == SEKEY_UNCHANGED){
					continue; // rollback done, try one more time
				}
			} else {
				continue; // in this case the db is in a consistent status, no rollback needed, give it one more try
			}
		}
	}
	return SEKEY_ERR; // this is in case we tried many times to process the update but we were not able to complete the update
}
int sekey_write_recovery(string& user_id, string& serial_number){
	SEfile updatefile;
	int pos, rc, cnt = 0;
	string query, sn, pin;
	statement sqlstmt, temp_stmt;
	unique_ptr<uint8_t[]> buf;
	uint32_t k1_id = 0, k2_id = 0, algo = 0, payloadsize, offset, recordsize;
	string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", recovery written for user " + user_id + " with SN " + serial_number;
	if(is_admin == false){ /* this API is available only to the administrator */
		return SEKEY_ERR_AUTH;
	}
	if(user_id.empty() || user_id.length()>IDLEN || !check_input(user_id, 0)){
		return SEKEY_ERR_PARAMS;
	}
	/* check if the user (given its serial number) is in the database, if not send a generic update to delete it */
	query = "SELECT COUNT(*) FROM Users WHERE user_id = ?1 AND serial_number = ?2;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 2, serial_number.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
		return SEKEY_ERR;
	}
	for(;;){
		if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			return SEKEY_ERR;
		}
		cnt = sqlite3_column_int64(sqlstmt.getstmt(), 0);
	}
	/* in case the user with that serial number is not in the database send an update encrypted with the wildcard key to delete the entire content.
	 * otherwise continue with normal operations... */
	if(cnt == 0){
		req_delete_user(user_id, L1Algorithms::Algorithms::AES_HMACSHA256, L1Key::Id::RESERVED_ID_SEKEY_WILDCARD, serial_number, true, RECOVERY);
		return SEKEY_OK;
	}
	if(cnt > 1){ // this should be impossible
		return SEKEY_ERR;
	}
	/* since we are performing a recovery, reset the update counter for the current user. notice that resetting the update counter is correct
	 * because the recovery implies deleting also the normal update file (if any), therefore the new normal update file generated after this recovery
	 * file will start again from 0 and the user will be able to perform all the updates. in case the recovery generation doesn't work, the update
	 * counter is not important because a new recovery will be generated as soon as possible. in case the user has requested the recovery, if we reset
	 * the update counter to 0 and then we are not able to perform the recovery and then we decide to skip the recovery for the user, the next updates
	 * may contain update counters which are wrong because they have been reset to 0 while the user is expecting a higher number. this, in the end,
	 * would lead to incorrect SEkey data from the user point of view (the admin is safe) so a new recovery would be required. */
	query = "UPDATE Users SET update_counter = 0 WHERE user_id = ?1 AND serial_number = ?2;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 2, serial_number.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
	   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
		return SEKEY_ERR;
	}
	/* step 1: retrieve the data needed to generate the recovery file for this user */
	query = "SELECT serial_number, secube_user_pin, k1, k2, key_algo FROM Users WHERE user_id = ?1;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
		return SEKEY_ERR;
	}
	for(;;){
		if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			return SEKEY_ERR;
		}
		sn = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
		pin = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
		k1_id = get_u32(sqlstmt.getstmt(), 2);
		k2_id = get_u32(sqlstmt.getstmt(), 3);
		algo = get_u32(sqlstmt.getstmt(), 4);
	}
	if((updatefile.secure_init(SEcube, k1_id, algo) != 0) || (open_update_file(updatefile, sn, true, false, RECOVERY) != SEKEY_OK)){
		return SEKEY_ERR;
	}
	string recoverypath = root + sn + ".recovery";
	/* since this is a recovery, the very first thing to do from the user point of view is to clean every table before restoring its content */
	query = "DELETE FROM Users; DELETE FROM SeKeys; DELETE FROM UserGroup; DELETE FROM Groups; DELETE FROM Recovery;";
	payloadsize = query.length();
	recordsize = payloadsize + UPDATE_RECORD_HEADER_LEN;
	buf = make_unique<uint8_t[]>(recordsize);
	memset(buf.get(), 0, recordsize);
	offset = 0;
	buf[offset] = SQL_QUERY;
	offset++;
	memset(buf.get()+offset, 0, 8);
	offset+=8;
	memcpy(buf.get()+offset, &payloadsize, 2);
	offset+=2;
	memcpy(buf.get()+offset, query.c_str(), payloadsize);
	if((updatefile.secure_seek(0, &pos, SEFILE_END) != 0) ||
	   (updatefile.secure_write(buf.get(), recordsize) != 0)){
		deletefile(&updatefile, recoverypath);
		return SEKEY_ERR;
	}
	/* step 2: for each user known by this user select the entry from the user table */
	query = "SELECT user_id, username, serial_number, k1, k2, key_algo FROM Users WHERE user_id IN (SELECT DISTINCT tab1.user_id FROM UserGroup tab1 WHERE tab1.group_id IN "
			"(SELECT tab2.group_id FROM UserGroup tab2 WHERE tab2.user_id = ?1));";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
		deletefile(&updatefile, recoverypath);
		return SEKEY_ERR;
	}
	for(;;){
		if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			deletefile(&updatefile, recoverypath);
			return SEKEY_ERR;
		}
		string str;
		string uid = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
		string uname = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
		string tempsn = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 2);
		uint32_t kid1 = get_u32(sqlstmt.getstmt(), 3);
		uint32_t kid2 = get_u32(sqlstmt.getstmt(), 4);
		uint32_t alg = get_u32(sqlstmt.getstmt(), 5);
		if(uid == user_id){ // for the current user we need the entire row with full details
			str = "INSERT INTO Users(user_id, username, serial_number, secube_user_pin, secube_admin_pin, k1, k2, key_algo, init_flag, update_counter) VALUES(?1, ?2, ?3, ?4, 'empty', ?5, ?6, ?7, 1, 0);"; // update counter is always reset to 0 for the recovery file, init flag always 1
			if((sqlite3_prepare_v2(db, str.c_str(), -1, temp_stmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 2, uname.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 3, tempsn.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 4, pin.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 5, kid1) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 6, kid2) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 7, alg) != SQLITE_OK) ||
			   (sqlite3_expanded_sql_wrapper(temp_stmt.getstmt(), str) != SEKEY_OK)){
				deletefile(&updatefile, recoverypath);
				return SEKEY_ERR;
			}
		} else { // for other users we only care about name and id
			str = "INSERT INTO Users(user_id, username, serial_number, secube_user_pin, secube_admin_pin, k1, k2, key_algo, init_flag, update_counter) VALUES(?1, ?2, 'empty', 'empty', 'empty', 0, 0, 0, 0, 0);";
			if((sqlite3_prepare_v2(db, str.c_str(), -1, temp_stmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 1, uid.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 2, uname.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
			   (sqlite3_expanded_sql_wrapper(temp_stmt.getstmt(), str) != SEKEY_OK)){
				deletefile(&updatefile, recoverypath);
				return SEKEY_ERR;
			}
		}
		payloadsize = str.length();
		recordsize = payloadsize + UPDATE_RECORD_HEADER_LEN;
		buf = make_unique<uint8_t[]>(recordsize);
		memset(buf.get(), 0, recordsize);
		offset = 0;
		buf[offset] = SQL_QUERY;
		offset++;
		memset(buf.get()+offset, 0, 8);
		offset+=8;
		memcpy(buf.get()+offset, &payloadsize, 2);
		offset+=2;
		memcpy(buf.get()+offset, str.c_str(), payloadsize);
		if((updatefile.secure_seek(0, &pos, SEFILE_END) != 0) ||
		   (updatefile.secure_write(buf.get(), recordsize) != 0)){
			deletefile(&updatefile, recoverypath);
			return SEKEY_ERR;
		}
	}
	/* for each group to which the user belongs, generate a SQL query */
	query = "SELECT * FROM Groups WHERE group_id IN (SELECT group_id FROM UserGroup WHERE user_id = ?1);";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
		deletefile(&updatefile, recoverypath);
		return SEKEY_ERR;
	}
	for(;;){
		if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			deletefile(&updatefile, recoverypath);
			return SEKEY_ERR;
		}
		string group_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
		string group_name = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
		uint32_t user_counter = get_u32(sqlstmt.getstmt(), 2);
		uint32_t keys_counter = get_u32(sqlstmt.getstmt(), 3);
		uint32_t keys_maxnumber = get_u32(sqlstmt.getstmt(), 4);
		uint32_t keys_algorithm = get_u32(sqlstmt.getstmt(), 5);
		uint32_t keys_liveness = get_u32(sqlstmt.getstmt(), 6);
		string str = "INSERT INTO Groups(group_id, group_name, users_counter, keys_counter, max_keys, algorithm, keys_liveness) VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7);";
		if((sqlite3_prepare_v2(db, str.c_str(), -1, temp_stmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(temp_stmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
		   (sqlite3_bind_text(temp_stmt.getstmt(), 2, group_name.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 3, user_counter)!=SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 4, keys_counter)!=SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 5, keys_maxnumber)!=SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 6, keys_algorithm)!=SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 7, keys_liveness)!=SQLITE_OK) ||
		   (sqlite3_expanded_sql_wrapper(temp_stmt.getstmt(), str) != SEKEY_OK)){
			deletefile(&updatefile, recoverypath);
			return SEKEY_ERR;
		}
		payloadsize = str.length();
		recordsize = payloadsize + UPDATE_RECORD_HEADER_LEN;
		buf = make_unique<uint8_t[]>(recordsize);
		memset(buf.get(), 0, recordsize);
		offset = 0;
		buf[offset] = SQL_QUERY;
		offset++;
		memset(buf.get()+offset, 0, 8);
		offset+=8;
		memcpy(buf.get()+offset, &payloadsize, 2);
		offset+=2;
		memcpy(buf.get()+offset, str.c_str(), payloadsize);
		if((updatefile.secure_seek(0, &pos, SEFILE_END) != 0) ||
		   (updatefile.secure_write(buf.get(), recordsize) != 0)){
			deletefile(&updatefile, recoverypath);
			return SEKEY_ERR;
		}
	}
	/* for each entry of the UserGroup table related to the user, generate a SQL query */
	query = "SELECT * FROM UserGroup tab1 WHERE tab1.group_id IN (SELECT tab2.group_id FROM UserGroup tab2 WHERE tab2.user_id = ?1);";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
		deletefile(&updatefile, recoverypath);
		return SEKEY_ERR;
	}
	for(;;){
		if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			deletefile(&updatefile, recoverypath);
			return SEKEY_ERR;
		}
		string str = "INSERT INTO UserGroup(user_id, group_id) VALUES(?1, ?2);";
		string tempuser = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
		string tempgroup = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
		if((sqlite3_prepare_v2(db, str.c_str(), -1, temp_stmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(temp_stmt.getstmt(), 1, tempuser.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
		   (sqlite3_bind_text(temp_stmt.getstmt(), 2, tempgroup.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
		   (sqlite3_expanded_sql_wrapper(temp_stmt.getstmt(), str) != SEKEY_OK)){
			deletefile(&updatefile, recoverypath);
			return SEKEY_ERR;
		}
		payloadsize = str.length();
		recordsize = payloadsize + UPDATE_RECORD_HEADER_LEN;
		buf = make_unique<uint8_t[]>(recordsize);
		memset(buf.get(), 0, recordsize);
		offset = 0;
		buf[offset] = SQL_QUERY;
		offset++;
		memset(buf.get()+offset, 0, 8);
		offset+=8;
		memcpy(buf.get()+offset, &payloadsize, 2);
		offset+=2;
		memcpy(buf.get()+offset, str.c_str(), payloadsize);
		if((updatefile.secure_seek(0, &pos, SEFILE_END) != 0) ||
		   (updatefile.secure_write(buf.get(), recordsize) != 0)){
			deletefile(&updatefile, recoverypath);
			return SEKEY_ERR;
		}
	}
	/* for each key belonging to a group to which the user belongs, add the SQL query to the update */
	query = "SELECT * FROM SeKeys WHERE key_owner IN (SELECT group_id FROM UserGroup WHERE user_id = ?1);";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
		deletefile(&updatefile, recoverypath);
		return SEKEY_ERR;
	}
	for(;;){
		if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			deletefile(&updatefile, recoverypath);
			return SEKEY_ERR;
		}
		string key_id, key_name, key_owner;
		key_id.assign(sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0));
		key_name.assign(sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1));
		key_owner.assign(sqlite3_column_text_wrapper(sqlstmt.getstmt(), 2));
		uint32_t key_status = get_u32(sqlstmt.getstmt(), 3);
		uint32_t key_algo = get_u32(sqlstmt.getstmt(), 4);
		uint32_t key_length = get_u32(sqlstmt.getstmt(), 5);
		time_t gen = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 6);
		time_t act = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 7);
		time_t exp = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 8);
		time_t crypto = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 9);
		time_t deactivation = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 10);
		uint32_t key_type = get_u32(sqlstmt.getstmt(), 11);
		time_t compr = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 12);
		time_t destr = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 13);
		time_t susp = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 14);
		string str = "INSERT INTO SeKeys(key_id, key_name, key_owner, status, algorithm, key_length, generation, activation, expiration, cryptoperiod, deactivation, type, compromise, destruction, suspension) "
				"VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15);";
		if((sqlite3_prepare_v2(db, str.c_str(), -1, temp_stmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(temp_stmt.getstmt(), 1, key_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
		   (sqlite3_bind_text(temp_stmt.getstmt(), 2, key_name.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
		   (sqlite3_bind_text(temp_stmt.getstmt(), 3, key_owner.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 4, key_status) != SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 5, key_algo) != SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 6, key_length) != SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 7, gen) != SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 8, act) != SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 9, exp) != SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 10, crypto) != SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 11, deactivation) != SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 12, key_type) != SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 13, compr) != SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 14, destr) != SQLITE_OK) ||
		   (sqlite3_bind_int64(temp_stmt.getstmt(), 15, susp) != SQLITE_OK) ||
		   (sqlite3_expanded_sql_wrapper(temp_stmt.getstmt(), str) != SEKEY_OK)){
			deletefile(&updatefile, recoverypath);
			return SEKEY_ERR;
		}
		payloadsize = str.length();
		recordsize = payloadsize + UPDATE_RECORD_HEADER_LEN;
		buf = make_unique<uint8_t[]>(recordsize);
		memset(buf.get(), 0, recordsize);
		offset = 0;
		buf[offset] = SQL_QUERY;
		offset++;
		memset(buf.get()+offset, 0, 8);
		offset+=8;
		memcpy(buf.get()+offset, &payloadsize, 2);
		offset+=2;
		memcpy(buf.get()+offset, str.c_str(), payloadsize);
		if((updatefile.secure_seek(0, &pos, SEFILE_END) != 0) ||
		   (updatefile.secure_write(buf.get(), recordsize) != 0)){
			deletefile(&updatefile, recoverypath);
			return SEKEY_ERR;
		}
		// write the key content to the update file (only if the key is not destroyed)
		if(key_status != (uint32_t)se_key_status::destroyed){
			shared_ptr<uint8_t[]> update_key; // key content to be stored on the user device
			uint16_t update_key_len;
			string ksub = key_id.substr(1);
			if(!SEcube->L1SEkey_GetKeyEnc(stoul_wrap(ksub), k2_id, update_key, update_key_len)){ // retrieve key content
				 // in case of error delete the update file and return error
				deletefile(&updatefile, recoverypath);
				return SEKEY_ERR;
			}
			if(update_key == nullptr){ // this to prevent warning from compiler on update_key.get() in memcpy....update_key is set in L1GetKeyEnc
				deletefile(&updatefile, recoverypath);
				return SEKEY_ERR;
			}
			payloadsize = update_key_len + 2 + 4; /* bufsize made of key ID (4B), key length (2B) and key data */
			recordsize = payloadsize + UPDATE_RECORD_HEADER_LEN;
			unique_ptr<uint8_t[]> buf2 = make_unique<uint8_t[]>(recordsize); // new buffer to be written to the file
			offset = 0;
			uint32_t _kid_ = stoul_wrap(ksub);
			buf2[offset] = KEY_DATA; // command type
			offset++;
			memset(buf2.get()+offset, 0, 8);
			offset+=8;
			memcpy(buf2.get()+offset, &payloadsize, 2); // payload length
			offset+=2;
			memcpy(buf2.get()+offset, &_kid_, 4); // key ID
			offset+=4;
			memcpy(buf2.get()+offset, &update_key_len, 2); // key length
			offset+=2;
			memcpy(buf2.get()+offset, update_key.get(), update_key_len); // key data
			if((updatefile.secure_seek(0, &pos, SEFILE_END) != 0) ||
			   (updatefile.secure_write(buf2.get(), recordsize) != 0)){
				deletefile(&updatefile, recoverypath);
				return SEKEY_ERR;
			}
		}
	}
	if(updatefile.secure_close() != 0){
		deletefile(&updatefile, recoverypath);
		return SEKEY_ERR;
	}
	/* delete any standard update file for the user, it is now useless since the recovery file contains the most recent data.
	 * warning: here we check if the file exists and then we perform the deletion. if the file is deleted bewteen the two operations, the
	 * deletefile will return an error and the recovery generation will fail. in a concurrent scenario (i.e. admin and user on different
	 * machines with a server that handles updates) the server MUST implement concurrent access logic.
	 * notice that if the update file is not deleted, an error will be returned and the recovery will be done again later. in the meantime,
	 * the user could read the recovery file and also the update file (not deleted here) so it would probably have wrong SEkey data in its
	 * database. this will be solved with the next recovery, issued as soon as possible. */
	string normalupdatefile = root + serial_number + ".normal";
	if((rc = file_exists(normalupdatefile)) == SEKEY_FILE_FOUND){
		if(!deletefile(nullptr, normalupdatefile)){ return SEKEY_ERR; }
	}
	if(rc != SEKEY_FILE_FOUND && rc != SEKEY_FILE_NOT_FOUND){
		return SEKEY_ERR;
	}
	sekey_printlog(msg);
	return SEKEY_OK;
}
int sekey_recovery(){
	/* Warning: by design each API of SEkey that implies a change in the database must process all the recoveries required by the system.
	 * If the system is not able to generate the recovery file for a device that needs to be recovered, the API must not be executed because
	 * otherwise the very same API could remove that user from the recovery table in case the API involves an update for that user and the
	 * update is written correctly (because the user to be updated is added to the recovery table in advance and then is deleted only if
	 * the update is written correctly, but if the user is already there because of an earlier recovery he should not be deleted...this
	 * can be solved in future adding a counter in the recovery table for each user so the user is deleted only if the counter is equal to
	 * 1). In general is better to force a severe approach on recovery because the administrator should issue the recovery as soon as possible.
	 * Another thing is that this API may throw, if it throws the recovery is to be considered not completed so the API must not be executed. */
	if(!is_admin){ return SEKEY_ERR_AUTH; }
	int rc;
	statement sqlstmt;
	bool failed = false;
	vector<pair<string,string>> torecover;
	se3_flash_maintenance_routine(); // clean the flash
	string query = "SELECT * FROM Recovery;";
	if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK){
		return SEKEY_ERR;
	}
	for(;;){
		if((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if(rc != SQLITE_ROW){
			return SEKEY_ERR;
		}
		string id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
		string sn = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
		torecover.push_back(pair<string,string>(id, sn));

	}
	for(pair<string,string> r : torecover){
		if(sekey_write_recovery(r.first, r.second) == SEKEY_OK){
			// recovery file correctly written, delete user from recovery table
			reset_user_recovery(r.first, r.second); // don't check if delete from recovery succeeded because next select count will check the entire recovery table for any leftover
		} else {
			failed = true;
		}
	}
	// to be sure that recovery has been done correctly for every user who was in the recovery table, count the number of rows in the recovery table
	query = "SELECT COUNT(*) FROM Recovery;";
	if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK){
		return SEKEY_ERR;
	}
	for(;;){
		if((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if(rc != SQLITE_ROW){
			return SEKEY_ERR;
		}
		if(sqlite3_column_int(sqlstmt.getstmt(), 0) != 0){
			failed = true;
		}
	}
	if(failed){
		return SEKEY_ERR;
	} else {
		return SEKEY_OK;
	}
	/* important: this function must return ok only if no user is left in the recovery table. otherwise the normal API of sekey could involve an update for a user who's in the recovery table, so the API would not
	 * add that user to the table before commit (because even assuming the worst case scenario the user is already in the table) but the same API would delete the user from the table. */
}
int sekey_recovery_request(string& user_id, string& serial_number){
	try{
		if(!is_admin){ return SEKEY_ERR; }
		string query;
		statement sqlstmt;
		int rc;
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", recovery requested for user " + user_id + " with SN " + serial_number;
		query = "INSERT OR IGNORE INTO Recovery(user_id, serial_number) VALUES(?1, ?2);";
		if((( rc = sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)) != SQLITE_OK) ||
		   (( rc = sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) ||
		   (( rc = sqlite3_bind_text(sqlstmt.getstmt(), 2, serial_number.c_str(), -1, SQLITE_STATIC)) != SQLITE_OK) ||
		   (( rc = sqlite3_step(sqlstmt.getstmt())) != SQLITE_DONE)){
			return SEKEY_ERR;
		}
		sekey_printlog(msg);
		if(sekey_write_recovery(user_id, serial_number) != SEKEY_OK){
			return SEKEY_ERR; // return if recovery file not created correctly
		}
		if(reset_user_recovery(user_id, serial_number) != SEKEY_OK){ // recovery done, remove user from recovery table
			return SEKEY_ERR;
		}
		return SEKEY_OK;
	} catch (...) {
		return SEKEY_ERR;
	}
}
int sekey_find_key_v1(string& chosen_key, string& dest_user_id, se_key_type keytype){
	try{
		string source_user_id = currentuser.userid;
		if(!user_allowed()){ return SEKEY_BLOCKED; } // block the user if the device is not updated.
		if(source_user_id.empty() || source_user_id.length()>IDLEN || dest_user_id.empty() || !check_input(source_user_id, 0)
			|| dest_user_id.length()>IDLEN || !check_input(dest_user_id, 0) || keytype<=se_key_type::typemin || keytype>=se_key_type::typemax){
			return SEKEY_ERR_PARAMS;
		}
		if(keytype != se_key_type::symmetric_data_encryption){
			return SEKEY_UNSUPPORTED;
		}
		// Notice that this section is commented to allow "private" encryption (i.e. I want to encrypt data for myself).
		// The way to do this is asking to the admin to create a group with a single user. If not possible, the key of the smallest group will be used so encryption won't be private.
		/*if(dest_user_id == source_user_id){
			return SEKEY_ERR_PARAMS;
		}*/
		uint32_t min_user_group = UINT32_MAX;
		int rc;
		statement sqlstmt;
		if((rc=sekey_check_expired_keys()) != SEKEY_OK){ return rc; } // the check for expired keys is done here
		if(((rc=is_user_present(source_user_id)) != SEKEY_OK) || ((rc=is_user_present(dest_user_id)) != SEKEY_OK)){ return rc; }
		vector<string> selected_groups, common_groups;
		string query, key_id, s;
		uint32_t key_algo, key_len;
		time_t key_act, key_exp;
		se_key chosen;
		// step 1: generate a list of groups which have source user and destination user in common and which have at least one active key
		query = "SELECT source.group_id FROM UserGroup source WHERE source.user_id = ?1 AND source.group_id IN "
				"(SELECT DISTINCT key_owner FROM SeKeys WHERE status = " + to_string((uint32_t)se_key_status::active) + ") "
				"AND source.group_id IN (SELECT usgr.group_id FROM UserGroup usgr WHERE usgr.user_id = ?2);";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, source_user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, dest_user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), s) != SEKEY_OK)){
				return SEKEY_ERR;
		}
		if(sql_fill_vector(nullptr, s, &common_groups) != SEKEY_OK){
			return SEKEY_ERR;
		}
		if(common_groups.empty()){
			return SEKEY_KEY_NOT_FOUND; // no common groups with active key
		}
		// step 3: filter the common groups keeping only the groups which have the minimum number of users
		for(string& selected_group : common_groups){
			query = "SELECT users_counter FROM Groups WHERE group_id = ?1;";
			if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 1, selected_group.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
					return SEKEY_ERR;
			}
			for(;;){
				if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
					break;
				}
				if (rc != SQLITE_ROW) {
					return SEKEY_ERR;
				}
				uint32_t curr_users = get_u32(sqlstmt.getstmt(), 0);
				if(curr_users < min_user_group){
					min_user_group = curr_users; // update the new minimum value
					selected_groups.clear(); // clear because we found a new minimum value
					selected_groups.push_back(selected_group);
				} else {
					if(curr_users == min_user_group){
						selected_groups.push_back(selected_group);
					}
				}
			}
		}
		if(selected_groups.empty()){
			return SEKEY_KEY_NOT_FOUND;
		}
		// step 4: retrieve the id of the active key of the selected group
		/* step 5: filter the key according to the following criteria:
				 * 1) smallest group
				 * 2) best algorithm (in terms of algorithm strength and key strength)
				 * 3) most recent key (because it is less probable that an attacker has intercepted many communications encrypted with this key)
				 * if still there are multiple keys, take the one with the smallest ID value */
		bool first = true, found = false;
		for(string& selected_group : selected_groups){
			query = "SELECT key_id, algorithm, key_length, activation, expiration FROM SeKeys WHERE status = " + to_string((uint32_t)se_key_status::active) + " AND key_owner = ?1;";
			if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 1, selected_group.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
					return SEKEY_ERR;
			}
			for(;;){
				if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){ // the query ended correctly
					break;
				}
				if (rc != SQLITE_ROW) { // query returned wrong value
					return SEKEY_ERR;
				}
				key_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
				key_algo = get_u32(sqlstmt.getstmt(), 1);
				key_len = get_u32(sqlstmt.getstmt(), 2);
				key_act = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 3);
				key_exp = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 4);
				if(first){
					first = false;
					se_key temp(key_id, key_algo, key_len, key_act, key_exp);
					chosen = temp;
					found = true;
				} else {
					se_key k(key_id, key_algo, key_len, key_act, key_exp);
					if(k.safer(chosen)){
						chosen = k;
						found = true;
					}
				}
			}
		}
		if(found == false){
			return SEKEY_KEY_NOT_FOUND;
		}
		chosen_key = chosen.get_id();
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", retrieved key " + chosen.get_id();
		sekey_printlog(msg);
		return SEKEY_OK;
	} catch(...){
		return SEKEY_ERR;
	}
}
int sekey_find_key_v2(string& chosen_key, string& group_id, se_key_type keytype){
	try{
		string source_user_id = currentuser.userid;
		if(!user_allowed()){ return SEKEY_BLOCKED; } // block the user if the device is not updated.
		if(source_user_id.empty() || source_user_id.length()>IDLEN || group_id.empty() || !check_input(source_user_id, 0) || !check_input(group_id, 1) || group_id.length()>IDLEN
				|| keytype<=se_key_type::typemin || keytype>=se_key_type::typemax){
			return SEKEY_ERR_PARAMS;
		}
		if(keytype != se_key_type::symmetric_data_encryption){
			return SEKEY_UNSUPPORTED;
		}
		int rc;
		statement sqlstmt;
		string query, key_id;
		uint32_t key_algo, key_len;
		time_t key_act, key_exp;
		se_key chosen;
		bool first = true, found = false;
		if((rc=sekey_check_expired_keys()) != SEKEY_OK){ return rc; }
		if((rc=is_user_present(source_user_id)) != SEKEY_OK){ return rc; }
		if((rc=is_group_present(group_id)) != SEKEY_OK){ return rc; }
		// retrieve the best active key of the specified group
		query = "SELECT key_id, algorithm, key_length, activation, expiration FROM SeKeys WHERE status = " + to_string((uint32_t)se_key_status::active) + " AND key_owner = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
				return SEKEY_ERR;
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){ // the query ended correctly
				break;
			}
			if (rc != SQLITE_ROW) { // query returned wrong value
				return SEKEY_ERR;
			}
			key_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			key_algo = get_u32(sqlstmt.getstmt(), 1);
			key_len = get_u32(sqlstmt.getstmt(), 2);
			key_act = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 3);
			key_exp = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 4);
			if(first){
				first = false;
				se_key temp(key_id, key_algo, key_len, key_act, key_exp);
				chosen = temp;
				found = true;
			} else {
				se_key k(key_id, key_algo, key_len, key_act, key_exp);
				if(k.safer(chosen)){
					chosen = k;
					found = true;
				}
			}
		}
		if(found == false){
			return SEKEY_KEY_NOT_FOUND;
		}
		chosen_key = chosen.get_id();
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", retrieved key " + chosen.get_id();
		sekey_printlog(msg);
		return SEKEY_OK;
	} catch(...){
		return SEKEY_ERR;
	}
}
int sekey_find_key_v3(string& chosen_key, vector<string>& dest_user_id, se_key_type keytype){
	try{
		string source_user_id = currentuser.userid;
		if(!user_allowed()){ return SEKEY_BLOCKED; } // block the user if the device is not updated.
		if(source_user_id.empty() || source_user_id.length()>IDLEN || dest_user_id.empty() || !check_input(source_user_id, 0) || keytype<=se_key_type::typemin || keytype>=se_key_type::typemax){
			return SEKEY_ERR_PARAMS;
		}
		for(string str : dest_user_id){
			if(str.length()>IDLEN || !check_input(str, 0)){
				return SEKEY_ERR_PARAMS;
			}
		}
		if(keytype != se_key_type::symmetric_data_encryption){
			return SEKEY_UNSUPPORTED;
		}
		uint32_t min_user_group = UINT32_MAX;
		int rc;
		statement sqlstmt;
		if((rc=sekey_check_expired_keys()) != SEKEY_OK){ return rc; }
		if((rc=is_user_present(source_user_id)) != SEKEY_OK){ return rc; }
		for(string str : dest_user_id){
			rc = is_user_present(str);
			if(rc !=SEKEY_OK){ return rc; }
		}
		vector<string> selected_groups, common_groups;
		map<string, unsigned> groupsmap;
		string query, key_id, s;
		uint32_t key_algo, key_len;
		time_t key_act, key_exp;
		se_key chosen;
		// create a map with each group that is shared by a specific couple source,dest. the key is the group, the value is the number of times that group is found to be in common
		for(string currdest : dest_user_id){
			vector<string> grouplist; // groups in common between the source and the current destination
			query = "SELECT source.group_id FROM UserGroup source WHERE source.user_id = ?1 AND source.group_id IN "
					"(SELECT DISTINCT key_owner FROM SeKeys WHERE status = " + to_string((uint32_t)se_key_status::active) + ")"
					" AND source.group_id IN (SELECT usgr.group_id FROM UserGroup usgr WHERE usgr.user_id = ?2);";
			if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 1, source_user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 2, currdest.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), s) != SEKEY_OK)){
					return SEKEY_ERR;
			}
			if(sql_fill_vector(nullptr, s, &grouplist) != SEKEY_OK){
				return SEKEY_ERR;
			}
			if(grouplist.empty()){ // no common groups with active key, useless to go on
				return SEKEY_KEY_NOT_FOUND;
			}
			for(string currgroup : grouplist){
				map<string, unsigned>::iterator it = groupsmap.find(currgroup);
				if(it != groupsmap.end()){
					it->second++;
				} else {
					groupsmap.insert(make_pair(currgroup, 1));
				}
			}
		}
		// when the map is full, keep only the groups which are common to the source and to all destinations
		map<string, unsigned>::iterator myit;
		for(myit = groupsmap.begin(); myit != groupsmap.end(); myit++){
			if(myit->second == dest_user_id.size()){
				common_groups.push_back(myit->first);
			}
		}
		if(common_groups.empty()){
			if(dest_user_id.size() == 1){
				return SEKEY_KEY_NOT_FOUND; // return this because it was a 1-to-1 key request
			} else {
				return SEKEY_COMMON_GROUP_NOT_FOUND; // return this if it was a multicast key request
			}
		}
		// filter the common groups keeping only the groups which have the minimum number of users
		for(string& selected_group : common_groups){
			query = "SELECT users_counter FROM Groups WHERE group_id = ?1;";
			if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 1, selected_group.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
					return SEKEY_ERR;
			}
			for(;;){
				if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
					break;
				}
				if (rc != SQLITE_ROW) {
					return SEKEY_ERR;
				}
				uint32_t curr_users = get_u32(sqlstmt.getstmt(), 0);
				if(curr_users < min_user_group){
					min_user_group = curr_users; // update the new minimum value
					selected_groups.clear(); // clear because we found a new minimum value
					selected_groups.push_back(selected_group);
				} else {
					if(curr_users == min_user_group){
						selected_groups.push_back(selected_group);
					}
				}
			}
		}
		if(selected_groups.empty()){
			return SEKEY_KEY_NOT_FOUND;
		}
		/* filter the key according to the following criteria:
				 * 1) smallest group
				 * 2) best algorithm (in terms of algorithm strength and key strength)
				 * 3) most recent key (because it is less probable that an attacker has intercepted many communications encrypted with this key)
				 * if still there are multiple keys, take the one with the smallest ID value */
		bool first = true, found = false;
		for(string& selected_group : selected_groups){
			query = "SELECT key_id, algorithm, key_length, activation, expiration FROM SeKeys WHERE status = " + to_string((uint32_t)se_key_status::active) + " AND key_owner = ?1;";
			if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 1, selected_group.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
					return SEKEY_ERR;
			}
			for(;;){
				if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){ // the query ended correctly
					break;
				}
				if (rc != SQLITE_ROW) { // query returned wrong value
					return SEKEY_ERR;
				}
				key_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
				key_algo = get_u32(sqlstmt.getstmt(), 1);
				key_len = get_u32(sqlstmt.getstmt(), 2);
				key_act = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 3);
				key_exp = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 4);
				if(first){
					first = false;
					se_key temp(key_id, key_algo, key_len, key_act, key_exp);
					chosen = temp;
					found = true;
				} else {
					se_key k(key_id, key_algo, key_len, key_act, key_exp);
					if(k.safer(chosen)){
						chosen = k;
						found = true;
					}
				}
			}
		}
		if(found == false){
			return SEKEY_ERR;
		}
		chosen_key = chosen.get_id();
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", retrieved key " + chosen.get_id();
		sekey_printlog(msg);
		return SEKEY_OK;
	} catch(...){
		return SEKEY_ERR;
	}
}

/* APIs to initialize the SEcube */
int sekey_init_user_SEcube(string& uid, array<uint8_t, L1Parameters::Size::PIN>& userpin, array<uint8_t, L1Parameters::Size::PIN>& adminpin, vector<array<uint8_t, L1Parameters::Size::PIN>>& pin){
	userdata udata;
	/* first part of user's SEcube initialization: retrieve details from administrator's SEcube */
	try{
		if(!is_admin){ return SEKEY_ERR_AUTH; }
		string query, username, user_id, serialnumber;
		statement sqlstmt;
		int rc;
		uint32_t k1 = 0, k2 = 0, algo = 0; /* k1 is the key to cipher the update, k2 is the key to cipher the keys within the update */
		bool found = false;
		/* step 1: retrieve from the admin the info needed to initialize the user's SEcube */
		query = "SELECT user_id, username, serial_number, k1, k2, key_algo FROM Users WHERE user_id = ?1;"; /* AND init_flag = 0 removed because we want to be able to initialize the SEcube multiple times (i.e. if the original SEcube is damaged) */
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, uid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
			return SEKEY_ERR;
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return SEKEY_ERR;
			}
			found = true;
			user_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			username = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
			serialnumber = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 2);
			k1 = get_u32(sqlstmt.getstmt(), 3);
			k2 = get_u32(sqlstmt.getstmt(), 4);
			algo = get_u32(sqlstmt.getstmt(), 5);
		}
		if(found == false){	return SEKEY_ERR; }
		udata.algo = algo;
		udata.k1 = k1;
		udata.k2 = k2;
		if(serialnumber.length()>L0Communication::Size::SERIAL){
			return SEKEY_ERR;
		} else {
			memcpy(udata.sn.data(), serialnumber.data(), serialnumber.length());
		}
		udata.uid = user_id;
		udata.uname = username;
		// retrieve the content of the keys dedicated to this user
		uint32_t klen = algolen(algo); // this is the length of the keys used to encrypt the updates for this user
		if(klen == 0){ return SEKEY_ERR; }
		udata.klen = klen;
		shared_ptr<uint8_t[]> p1(new uint8_t[klen]);
		shared_ptr<uint8_t[]> p2(new uint8_t[klen]);
		shared_ptr<uint8_t[]> p3(new uint8_t[klen]);
		memset(p1.get(), 0, klen);
		memset(p2.get(), 0, klen);
		memset(p3.get(), 0, klen);
		udata.k1_data = std::move(p1);
		udata.k2_data = std::move(p2);
		udata.wcard_key = std::move(p3);
		/* export first update key from admin SEcube */
		shared_ptr<uint8_t[]> wrapped;
		uint16_t wrappedlen;
		size_t outlen = 0;
		if(!SEcube->L1SEkey_GetKeyEnc(k1, k1, wrapped, wrappedlen)){
			return SEKEY_ERR;
		}
		if(wrapped == nullptr){ return SEKEY_ERR; }
		SEcube_ciphertext ciphert;
		ciphert.reset();
		ciphert.algorithm = L1Algorithms::Algorithms::AES;
		ciphert.mode = CryptoInitialisation::Modes::CBC;
		ciphert.key_id = k1;
		for(int i=0; i<B5_AES_BLK_SIZE; i++){ // copy initialisation vector from wrapped data
			ciphert.initialization_vector[i] = wrapped[i];
		}
		ciphert.ciphertext_size = wrappedlen - B5_AES_BLK_SIZE;
		ciphert.ciphertext = make_unique<uint8_t[]>(ciphert.ciphertext_size); // actual ciphertext
		memcpy(ciphert.ciphertext.get(), wrapped.get()+B5_AES_BLK_SIZE, ciphert.ciphertext_size);
		shared_ptr<uint8_t[]> unwrapped;
		SEcube->L1Decrypt(ciphert, outlen, unwrapped);
		if(outlen != klen){
			return SEKEY_ERR;
		}
		if(unwrapped == nullptr){ return SEKEY_ERR; } // put here just to avoid warning on next line (arg 2 null when non-null expected)
		memcpy(udata.k1_data.get(), unwrapped.get(), klen);
		/* export second update key from admin SEcube */
		wrapped.reset();
		unwrapped.reset();
		if(!SEcube->L1SEkey_GetKeyEnc(k2, k2, wrapped, wrappedlen)){
			return SEKEY_ERR;
		}
		if(wrapped == nullptr){ return SEKEY_ERR; } // put here just to avoid warning on next line (arg 2 null when non-null expected)
		outlen = 0;
		ciphert.reset();
		ciphert.algorithm = L1Algorithms::Algorithms::AES;
		ciphert.mode = CryptoInitialisation::Modes::CBC;
		ciphert.key_id = k2;
		for(int i=0; i<B5_AES_BLK_SIZE; i++){ // copy initialisation vector from wrapped data
			ciphert.initialization_vector[i] = wrapped[i];
		}
		ciphert.ciphertext_size = wrappedlen - B5_AES_BLK_SIZE;
		ciphert.ciphertext = make_unique<uint8_t[]>(ciphert.ciphertext_size); // actual ciphertext
		memcpy(ciphert.ciphertext.get(), wrapped.get()+B5_AES_BLK_SIZE, ciphert.ciphertext_size);
		SEcube->L1Decrypt(ciphert, outlen, unwrapped);
		if(outlen != klen){
			return SEKEY_ERR;
		}
		if(unwrapped == nullptr){ return SEKEY_ERR; } // put here just to avoid warning on next line (arg 2 null when non-null expected)
		memcpy(udata.k2_data.get(), unwrapped.get(), klen);
		/* export wildcard key from admin SEcube */
		wrapped.reset();
		unwrapped.reset();
		if(!SEcube->L1SEkey_GetKeyEnc(L1Key::Id::RESERVED_ID_SEKEY_WILDCARD, L1Key::Id::RESERVED_ID_SEKEY_WILDCARD, wrapped, wrappedlen)){
			return SEKEY_ERR;
		}
		if(wrapped == nullptr){ return SEKEY_ERR; } // put here just to avoid warning on next line (arg 2 null when non-null expected)
		outlen = 0;
		ciphert.reset();
		ciphert.algorithm = L1Algorithms::Algorithms::AES;
		ciphert.mode = CryptoInitialisation::Modes::CBC;
		ciphert.key_id = L1Key::Id::RESERVED_ID_SEKEY_WILDCARD;
		for(int i=0; i<B5_AES_BLK_SIZE; i++){ // copy initialisation vector from wrapped data
			ciphert.initialization_vector[i] = wrapped[i];
		}
		ciphert.ciphertext_size = wrappedlen - B5_AES_BLK_SIZE;
		ciphert.ciphertext = make_unique<uint8_t[]>(ciphert.ciphertext_size); // actual ciphertext
		memcpy(ciphert.ciphertext.get(), wrapped.get()+B5_AES_BLK_SIZE, ciphert.ciphertext_size);
		SEcube->L1Decrypt(ciphert, outlen, unwrapped);
		if(outlen != klen){
			return SEKEY_ERR;
		}
		if(unwrapped == nullptr){ return SEKEY_ERR; } // put here just to avoid warning on next line (arg 2 null when non-null expected)
		memcpy(udata.wcard_key.get(), unwrapped.get(), klen);
	} catch (...) {
		return SEKEY_ERR;
	}
	/* end of step 1 */
	/* step 2: login to the user's SEcube and initialize it with the proper data */
	try{
		unique_ptr<L0> l0 = make_unique<L0>();
		uint8_t index = 0;
		string s;
		bool found = false;
		if(l0->GetNumberDevices() != 2){
			return SEKEY_ERR;
		}
		for(int i=0; i<l0->GetNumberDevices(); i++){
#if defined(__linux__) || defined(__APPLE__)
			char *c = (char*)l0->GetDevicePath();
			s = string(c);
			if(s.back() != '/'){
				s.push_back('/'); // add slash if required
			}
#elif _WIN32
			size_t retval;
			char path[L0Communication::Parameter::SE3_MAX_PATH];
			memset(path, '\0', L0Communication::Parameter::SE3_MAX_PATH);
			if(wcstombs_s(&retval, path, L0Communication::Parameter::SE3_MAX_PATH, l0->GetDevicePath(), L0Communication::Parameter::SE3_MAX_PATH-1) != 0){ // convert from wchar_t
				return SEKEY_ERR;
			}
			s = string(path);
			if(s.back() != '\\'){
				s.push_back('\\'); // add backslash if required
			}
#endif
			if(s.compare(SEcube_root) != 0){
				found = true;
				index = (uint8_t)i;
				break;
			} else {
				if(i < (l0->GetNumberDevices() - 1)){
					l0->SwitchToDevice(i+1);
				}
			}
		}
		if(!found){ return SEKEY_ERR; }
		unique_ptr<L1> l1 = make_unique<L1>(index); // L1 for a specific SEcube (the one just connected)
		try{
			l1->L1FactoryInit(udata.sn);
			// try all pins in the pin vector, hoping one of them is the correct pin to access to the user SEcube as admin
			for(array<uint8_t, L1Parameters::Size::PIN>& p : pin){
				try{
					l1->L1Login(p, SE3_ACCESS_ADMIN, true);
					break; // reaching the break means successful login
				} catch (...) {
					continue; // this is executed in case the login fails
				}
			}
			if(!l1->L1GetSessionLoggedIn()){ // all pins are wrong
				throw "generic error"; // used simply to jump to the next catch
			}
			l1->L1SetUserPIN(userpin);
			l1->L1SetAdminPIN(adminpin);
			// store user's informations in the flash
			if(!l1->L1SEkey_Info(udata.uid, udata.uname, L1SEkey::Direction::STORE)){
				throw "generic error"; // used simply to jump to the next catch
			}
			// store user data in user's SEcube
			if(!l1->L1SEkey_InsertKey(L1Key::Id::RESERVED_ID_SEKEY_SECUREDB, AES256KEYLEN, 0, nullptr) || // private key used for SEkey
			   !l1->L1SEkey_InsertKey(L1Key::Id::RESERVED_ID_SETELEGRAM, AES256KEYLEN, 0, nullptr) || // private key for SEtelegram db (if needed)
			   !l1->L1SEkey_InsertKey(L1Key::Id::RESERVED_ID_SEKEY_WILDCARD, udata.klen, 0, udata.wcard_key) || // wildcard key that will be used by the admin if needed
			   !l1->L1SEkey_InsertKey(udata.k1, udata.klen, 0, udata.k1_data) || // key for encrypted updates
			   !l1->L1SEkey_InsertKey(udata.k2, udata.klen, 0, udata.k2_data)){ // key for encrypted keys inside updates
				throw "generic error"; // used simply to jump to the next catch
			}
			l1->L1Logout();
		} catch (...) { // inner catch implies user SEcube flash is dirty and needs erase
			return SEKEY_REPROG;
		}
	} catch (...) {
		return SEKEY_ERR;
	}
	/* end of step 2*/
	/* step 3: update the database of the administrator */
	try{
		statement sqlstmt;
		// generate the query to be written in the update file for the user
		string query = "INSERT INTO Users(user_id, username, serial_number, secube_user_pin, secube_admin_pin, k1, k2, key_algo, init_flag, update_counter) VALUES(?1, ?2, ?3, ?4, 'empty', ?5, ?6, ?7, 1, 0);";
		string userpin_, adminpin_, sn_;
		for(uint8_t n : userpin){
			char c = n;
			userpin_.push_back(c);
		}
		for(uint8_t n : adminpin){
			char c = n;
			adminpin_.push_back(c);
		}
		for(uint8_t n : udata.sn){
			char c = n;
			sn_.push_back(c);
		}
		if( (sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)!=SQLITE_OK) ||
			(sqlite3_bind_text(sqlstmt.getstmt(), 1, udata.uid.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
			(sqlite3_bind_text(sqlstmt.getstmt(), 2, udata.uname.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
			(sqlite3_bind_text(sqlstmt.getstmt(), 3, sn_.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			(sqlite3_bind_text(sqlstmt.getstmt(), 4, userpin_.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 5, udata.k1) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 6, udata.k2) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 7, udata.algo)!=SQLITE_OK) ||
			(sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), query) != SEKEY_OK)){
			return SEKEY_ERR;
		}
		udata.query = query;
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_REPROG; }
		string sql = "UPDATE Users SET secube_user_pin = ?1, secube_admin_pin = ?2, init_flag = 1 WHERE user_id = ?3;";
		if((sqlite3_prepare_v2(db, sql.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, userpin_.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, adminpin_.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 3, udata.uid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			throw "generic error"; // used simply to jump to the next catch
		}
		string s = "DELETE FROM Users; DELETE FROM SeKeys; DELETE FROM UserGroup; DELETE FROM Groups; DELETE FROM Recovery;" + udata.query; // to be sure the db of the new user is empty before starting
		if(send_user_init_update(udata.uid, s) != SEKEY_OK){
			throw "generic error"; // used simply to jump to the next catch
		}
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", initialized SEcube of user " + udata.uid;
		int rc = commit_transaction();
		if(rc == SEKEY_OK){
			sekey_printlog(msg);
			return SEKEY_OK;
		} else {
			return rc;
		}
	} catch (...) {
		if(sqlite3_get_autocommit(db) == 0){
			if(rollback_transaction() == SEKEY_RESTART){
				return SEKEY_RESTART_REPROG;
			} else {
				return SEKEY_REPROG;
			}
		} else {
			return SEKEY_REPROG;
		}
	}
	/* end of step 3 */
}
int sekey_admin_init(L1& l1, vector<array<uint8_t, L1Parameters::Size::PIN>>& pin, array<uint8_t, L1Parameters::Size::PIN>& userpin, array<uint8_t, L1Parameters::Size::PIN>& adminpin){
	try{
		// generate the serial number, this default value will be partially overriden with a unique value making the serial number unique
		array<uint8_t, L0Communication::Size::SERIAL> SerialNumber;
		if(generate_serial_number(SerialNumber) != 0){
			return SEKEY_UNCHANGED;
		}
		// initialize the secube
		l1.L1FactoryInit(SerialNumber);
		for(array<uint8_t, L1Parameters::Size::PIN> p : pin){
			try{
				l1.L1Login(p, SE3_ACCESS_ADMIN, true);
				break; // reaching the break means successful login
			} catch (...) {
				continue; // this is executed in case the login fails
			}
		}
		if(!l1.L1GetSessionLoggedIn()){
			throw "generic error"; // simply to jump to the catch routing
		}
		// setup user pin and admin pin
		l1.L1SetUserPIN(userpin);
		l1.L1SetAdminPIN(adminpin);
		// store admin info in the device
		string id = "admin";
		string name = "admin";
		if(!l1.L1SEkey_Info(id, name, L1SEkey::Direction::STORE)){
			l1.L1Logout();
			return SEKEY_REPROG;
		}
		// generate the key to encrypt the SEkey database
		if(!l1.L1SEkey_InsertKey(L1Key::Id::RESERVED_ID_SEKEY_SECUREDB, AES256KEYLEN, 0, nullptr)){
			l1.L1Logout();
			return SEKEY_REPROG;
		}
		// generate wildcard key to cipher update in case there's no appropriate key in the db
		if(!l1.L1SEkey_InsertKey(L1Key::Id::RESERVED_ID_SEKEY_WILDCARD, AES256KEYLEN, 0, nullptr)){
			l1.L1Logout();
			return SEKEY_REPROG;
		}
		l1.L1Logout();
		return SEKEY_OK;
	} catch(...){ // catch whatever happens
		if(l1.L1GetSessionLoggedIn()){
			l1.L1Logout();
		}
		return SEKEY_REPROG;
	}
}

/* USER RELATED APIs */
int sekey_get_user_ids(vector<string>& ids){
	try{
		if(!user_allowed()){ return SEKEY_BLOCKED; }
		string query = "SELECT user_id FROM Users;";
		string id;
		statement sqlstmt;
		int rc;
		if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK){
			return SEKEY_ERR;
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return SEKEY_ERR;
			}
			id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			ids.push_back(id);
		}
		return SEKEY_OK;
	} catch (...) {
		return SEKEY_ERR;
	}
}
int sekey_add_user(string& user_id, string& username){
	if(!is_admin){ /* if the host software is not running with the secube of the admin return an error */
		return SEKEY_ERR_AUTH;
	}
	// check if the parameters are correct
	if(user_id.empty() || username.empty() || user_id.length()>IDLEN || username.length()>NAMELEN || !check_input(user_id, 0)){
		return SEKEY_ERR_PARAMS; // check parameters (the caller is responsible for passing a reasonable username, which is not checked in order to give more freedom to the user)
	}
	try{
		string sn;
		int rc;
		array<uint8_t, L0Communication::Size::SERIAL> SerialNumber;
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", added user " + user_id;
		if((rc = is_user_present(user_id)) == SEKEY_OK){ return SEKEY_USER_DUP; }
		if(rc != SEKEY_USER_NOT_FOUND){	return SEKEY_UNCHANGED; }
		for(;;){
			/* generate the serial number, this default value will be partially overriden
			 * with a unique value making the serial number unique */
			if(generate_serial_number(SerialNumber) != 0){ return SEKEY_UNCHANGED; }
			for(int i=0; i<L0Communication::Size::SERIAL; i++){
				sn.push_back(SerialNumber[i]);
			}
			/* to be sure to avoid any mistake, check if the serial number is unique */
			string query = "SELECT COUNT(*) FROM Users WHERE serial_number = ?1;";
			statement sqlstmt;
			bool unique = false;
			if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 1, sn.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
				return SEKEY_UNCHANGED;
			}
			for(;;){
				if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
					break;
				}
				if (rc != SQLITE_ROW) {
					return SEKEY_UNCHANGED;
				}
				if(sqlite3_column_int(sqlstmt.getstmt(), 0) == 0){
					unique = true;
				}
			}
			if(unique){ // serial number is unique
				break;
			}
		}
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){
			return SEKEY_UNCHANGED; // start transaction needed for admin db
		}
		// add the user to SEkey database of the administrator
		rc = sekey_user_init(user_id, username, sn); // this is the function that will modify the database
		if(rc == SEKEY_OK){
			rc = commit_transaction();
			if(rc == SEKEY_OK){
				sekey_printlog(msg);
			}
			return rc;
		} else {
			return rollback_transaction();
		}
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction();
		}
	}
}
int sekey_user_init(string& user_id, string& username, string& sn){
	if(is_admin == false){
		return SEKEY_ERR_AUTH;
	}
	string query;
	int rc;
	statement sqlstmt;
	uint32_t max = 0, k1_id = 0, k2_id = 0;
	bool found = false;
	/* step 1: retrieve the current maximum value for the id of the key used to cipher the keys inside the update file. the 2 keys required by the update
	 * file are generated as consecutive numbers, i.e. 100, 101. Then for the next user the ids will be 102, 103... */
	query = "SELECT MAX(k2) FROM Users;";
	if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)!=SQLITE_OK){
		return SEKEY_ERR;
	}
	for(;;){
		if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			return SEKEY_ERR;
		}
		max = get_u32(sqlstmt.getstmt(), 0);
	}
	if((L1Key::Id::RESERVED_ID_SEKEY_END - max) < 2){ // if there is no more space for 2 additional keys
		for(uint32_t j = L1Key::Id::RESERVED_ID_SEKEY_BEGIN; j < L1Key::Id::RESERVED_ID_SEKEY_END; j++){
			query = "SELECT COUNT(*) FROM Users WHERE k1 = " + to_string(j) + " OR k2 = " + to_string(j) + " OR k1 = " + to_string(j+1) + " OR k2 = " + to_string(j+1) + ";";
			if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)!=SQLITE_OK){
				return SEKEY_ERR;
			}
			for(;;){
				if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
					break;
				}
				if (rc != SQLITE_ROW) {
					return SEKEY_ERR;
				}
				if(sqlite3_column_int64(sqlstmt.getstmt(), 0) == 0){
					k1_id = j;
					k2_id = j+1;
					found = true;
				}
			}
			if(found){
				break;
			}
		}
		if(!found){
			return SEKEY_ERR;
		}
	} else {
		if(max == 0){
			k1_id = L1Key::Id::RESERVED_ID_SEKEY_BEGIN; // keep track of the key numeration offset only the first time
		} else {
			k1_id = max + 1;
		}
		k2_id = (k1_id + 1);
	}
	// Generate the unique admin-user keys for updates
	uint32_t klen = algolen(L1Algorithms::Algorithms::AES_HMACSHA256);
	if(klen == 0){
		return SEKEY_ERR;
	}
	if(!SEcube->L1SEkey_InsertKey(k1_id, klen, 0, nullptr) ||
	   !SEcube->L1SEkey_InsertKey(k2_id, klen, 0, nullptr)){
		return SEKEY_ERR;
	}
	// step 2: insert the new user in the system
	query = "INSERT INTO Users(user_id, username, serial_number, secube_user_pin, secube_admin_pin, k1, k2, key_algo, init_flag, update_counter) VALUES(?1, ?2, ?3, 'empty', 'empty', ?4, ?5, ?6, 0, 0);";
	if( (sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)!=SQLITE_OK) ||
		(sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
		(sqlite3_bind_text(sqlstmt.getstmt(), 2, username.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
		(sqlite3_bind_text(sqlstmt.getstmt(), 3, sn.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		(sqlite3_bind_int64(sqlstmt.getstmt(), 4, k1_id) != SQLITE_OK) ||
		(sqlite3_bind_int64(sqlstmt.getstmt(), 5, k2_id) != SQLITE_OK) ||
		(sqlite3_bind_int64(sqlstmt.getstmt(), 6, L1Algorithms::Algorithms::AES_HMACSHA256)!=SQLITE_OK) ||
		(sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
	}
	return SEKEY_OK;
}
int sekey_add_user_group(string& user_id, string& group_id){
	try{
		int rc;
		if(!is_admin){ /* this API is available only to the administrator */
			return SEKEY_ERR_AUTH;
		}
		if(user_id.empty() || group_id.empty() || user_id.length()>IDLEN || group_id.length()>IDLEN || !check_input(user_id, 0) || !check_input(group_id, 1)){
			return SEKEY_ERR_PARAMS; // input parameters must not be empty or longer than their maximum value, both IDs must match the expected regex
		}
		if(((rc = is_user_present(user_id)) != SEKEY_OK) || ((rc = is_group_present(group_id)) != SEKEY_OK)){ return rc; } // check if user and group are in the database
		if(sekey_recovery() != SEKEY_OK){ return SEKEY_UNCHANGED; } /* perform recovery of users who need it before going on with the API */
		if((rc=sekey_check_expired_keys()) != SEKEY_OK){ return rc; }
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_UNCHANGED; } /* start the transaction */
		statement sqlstmt;
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", added user " + user_id + " to group " + group_id;
		bool found = false;
		string query, update_new_user = "", update_old_user = "", additional_info;
		vector<string> users;
		vector<pair<uint32_t, uint32_t>> keys; // array of pairs <key id, key length> to be used to send all the keys to the new user
		/* The users who are already in this group do not know anything about the new user if they do not have already another group in common.
		 * Therefore retrieve the name of this user and generate the query to update the content of those other users of the same group who do not
		 * have the user in their Users table. */
		query = "SELECT username FROM Users WHERE user_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
				return rollback_transaction();
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return rollback_transaction();
			}
			string username = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			statement temp_stmt;
			string temp_str = "INSERT INTO Users(user_id, username, serial_number, secube_user_pin, secube_admin_pin, k1, k2, key_algo, init_flag, update_counter) VALUES(?1, ?2, 'empty', 'empty', 'empty', 0, 0, 0, 0, 0);";
			if((sqlite3_prepare_v2(db, temp_str.c_str(), -1, temp_stmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 2, username.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			   (sqlite3_expanded_sql_wrapper(temp_stmt.getstmt(), additional_info) != SEKEY_OK)){
				return rollback_transaction();
			}
			/* Notice that the 3rd and 4th parameters are set to empty or invalid values because these parameters are not actually used by SEkey, they
			 * are used only by the user himself. This means that, in the table Users, the user with id 1 will care only about the 3rd and 4th parameters
			 * of himself and will never use the parameters of other users. */
		}
		// step 1: retrieve all info about the group of the user (these data must be sent to the new user)
		query = "SELECT * FROM Groups WHERE group_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)!=SQLITE_OK) || (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
			return rollback_transaction();
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return rollback_transaction();
			}
			found = true;
			string gid = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			string gname = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
			uint32_t users_counter = get_u32(sqlstmt.getstmt(), 2);
			uint32_t keys_counter = get_u32(sqlstmt.getstmt(), 3);
			uint32_t keys_maxnumber = get_u32(sqlstmt.getstmt(), 4);
			uint32_t keys_algorithm = get_u32(sqlstmt.getstmt(), 5);
			uint32_t keys_liveness = get_u32(sqlstmt.getstmt(), 6);
			string temp = "INSERT INTO Groups(group_id, group_name, users_counter, keys_counter, max_keys, algorithm, keys_liveness) VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7);";
			statement temp_stmt;
			string s_;
			if((sqlite3_prepare_v2(db, temp.c_str(), -1, temp_stmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 1, gid.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)	||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 2, gname.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 3, users_counter)!=SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 4, keys_counter)!=SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 5, keys_maxnumber)!=SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 6, keys_algorithm)!=SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 7, keys_liveness)!=SQLITE_OK) ||
			   (sqlite3_expanded_sql_wrapper(temp_stmt.getstmt(), s_) != SEKEY_OK)){
					return rollback_transaction();
			}
			update_new_user.append(s_);
		}
		if(found == false){ // this means there is no such group
			return rollback_transaction();
		}
		// step 2: fill list of the users who already belong to the group (sql_fill_vector will do the binding)
		query = "SELECT user_id FROM UserGroup WHERE group_id = ?1;";
		if(sql_fill_vector(&group_id, query, &users) != 0){
			return rollback_transaction();
		}
		// step 3: select the users who already belong to the group and do not have any other group in common with the new user
		vector<string> single_users;
		query = "SELECT t1.user_id FROM UserGroup t1 WHERE t1.group_id = ?1 AND t1.user_id NOT IN (SELECT t2.user_id FROM UserGroup t2 WHERE t2.group_id IN ("
				"SELECT t3.group_id FROM UserGroup t3 WHERE t3.user_id = ?2));";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
				return rollback_transaction();
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return rollback_transaction();
			}
			string temp = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			single_users.push_back(temp);
		}
		// step 4: precompute update for the new user
		for(string& single_user : single_users){
			query = "SELECT username FROM Users WHERE user_id = ?1;";
			if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 1, single_user.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
				return rollback_transaction();
			}
			for(;;){
				if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
					break;
				}
				if (rc != SQLITE_ROW) {
					return rollback_transaction();
				}
				string uname = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
				statement temp_stmt;
				string s_;
				string temp_str = "INSERT INTO Users(user_id, username, serial_number, secube_user_pin, secube_admin_pin, k1, k2, key_algo, init_flag, update_counter) VALUES(?1, ?2, 'empty', 'empty', 'empty', 0, 0, 0, 0, 0);";
				if((sqlite3_prepare_v2(db, temp_str.c_str(), -1, temp_stmt.getstmtref(), nullptr) != SQLITE_OK) ||
				   (sqlite3_bind_text(temp_stmt.getstmt(), 1, single_user.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
				   (sqlite3_bind_text(temp_stmt.getstmt(), 2, uname.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
				   (sqlite3_expanded_sql_wrapper(temp_stmt.getstmt(), s_) != SEKEY_OK)){
						return rollback_transaction();
				}
				update_new_user.append(s_);
			}
		}
		// step 3: select all the entries of UserGroup table that involve users of this group (these entries must be replicated in the UserGroup table of the new user)
		query = "SELECT * FROM UserGroup WHERE group_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)!=SQLITE_OK) || (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
			return rollback_transaction();
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return rollback_transaction();
			}
			string uid = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			string ugroup = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
			statement temp_stmt;
			string s_;
			string temp_str = "INSERT INTO UserGroup(user_id, group_id) VALUES(?1, ?2);";
			if((sqlite3_prepare_v2(db, temp_str.c_str(), -1, temp_stmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 1, uid.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 2, ugroup.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
			   (sqlite3_expanded_sql_wrapper(temp_stmt.getstmt(), s_) != SEKEY_OK)){
				return rollback_transaction();
			}
			update_new_user.append(s_);
		}
		// step 4: select all info about the keys of this group
		query = "SELECT * FROM SeKeys WHERE key_owner = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)!=SQLITE_OK) || (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
			return rollback_transaction();
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return rollback_transaction();
			}
			string kid = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			string kname = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
			string kgroup = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 2);
			uint32_t status = get_u32(sqlstmt.getstmt(), 3);
			uint32_t algo = get_u32(sqlstmt.getstmt(), 4);
			uint32_t key_length = get_u32(sqlstmt.getstmt(), 5);
			time_t gen = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 6);
			time_t act = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 7);
			time_t exp = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 8);
			time_t crypto = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 9);
			time_t deactivation = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 10);
			uint32_t key_type = get_u32(sqlstmt.getstmt(), 11);
			time_t compr = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 12);
			time_t destr = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 13);
			time_t susp = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 14);
			statement temp_stmt;
			string s_;
			string temp_str = "INSERT INTO SeKeys(key_id, key_name, key_owner, status, algorithm, key_length, generation, activation, expiration, cryptoperiod, deactivation, type, compromise, destruction, suspension) "
					"VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15);";
			if((sqlite3_prepare_v2(db, temp_str.c_str(), -1, temp_stmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 1, kid.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 2, kname.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
			   (sqlite3_bind_text(temp_stmt.getstmt(), 3, kgroup.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 4, status) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 5, algo) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 6, key_length) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 7, gen) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 8, act) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 9, exp) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 10, crypto) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 11, deactivation) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 12, key_type) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 13, compr) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 14, destr) != SQLITE_OK) ||
			   (sqlite3_bind_int64(temp_stmt.getstmt(), 15, susp) != SQLITE_OK) ||
			   (sqlite3_expanded_sql_wrapper(temp_stmt.getstmt(), s_) != SEKEY_OK)){
					return rollback_transaction();
			}
			update_new_user.append(s_);
			// insert required key details in the list to be used later (only if the key is not in destroyed status)
			if(status != (uint32_t)se_key_status::destroyed){
				string ksub = kid.substr(1);
				keys.push_back(pair<uint32_t, uint32_t> (stoul_wrap(ksub), key_length));
			}
		}
		// step 4: generate updates for all users who where already part of the group
		vector<string> unknown_users; // this is the list of users who already belong to the group and do not have any other group in common with the user to be added to the group
		for(string& user : users){
			query = "SELECT COUNT(*) FROM UserGroup tab1 WHERE tab1.user_id = ?1 AND tab1.group_id IN (SELECT tab2.group_id FROM UserGroup tab2 WHERE tab2.user_id = ?2);";
			if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 2, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
					// if this does not work, remember that there was a continue instead of a return (don't remember why)
					return rollback_transaction();
			}
			for(;;){
				if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
					break;
				}
				if (rc != SQLITE_ROW) {
					return rollback_transaction();
				}
				if(sqlite3_column_int64(sqlstmt.getstmt(), 0) == 0){
					unknown_users.push_back(user);
				}
			}
		}
		// step 5: add the user to the group
		string s_;
		query = "INSERT INTO UserGroup(user_id, group_id) VALUES(?1, ?2);";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   ((rc = sqlite3_step(sqlstmt.getstmt())) != SQLITE_DONE) ||
		   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), s_) != SEKEY_OK)){
				if(rollback_transaction() == SEKEY_RESTART){ return SEKEY_RESTART; }
				if(rc == SQLITE_CONSTRAINT_PRIMARYKEY){ // database constraint violation (i.e. primary key already in use)
					return SEKEY_USER_GROUP_DUP;
				}
				return SEKEY_UNCHANGED;
		}
		update_old_user.append(s_);
		// step 6: update the counter of the users for the group
		query =	"UPDATE Groups SET users_counter = users_counter + 1 WHERE group_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)	||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE) ||
		   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), s_) != SEKEY_OK)){
			return rollback_transaction();
		}
		update_old_user.append(s_);
		vector<string> tempv;
		tempv.push_back(user_id);
		if((fill_recovery(users) != SEKEY_OK) || (fill_recovery(tempv) != SEKEY_OK)){
			return rollback_transaction();
		}
		if((rc=commit_transaction()) != SEKEY_OK){ return rc; } // transaction commit
		sekey_printlog(msg);
		try{ // use an inner try-catch to ignore exceptions that happen while sending the update
			// step 7: generate updates for all users who where already part of the group
			for(string& user : users){
				if(find(unknown_users.begin(), unknown_users.end(), user) != unknown_users.end()){
					string fullstring = update_old_user + additional_info;
					send_sql_update(user, fullstring, true);
				} else {
					send_sql_update(user, update_old_user, true);
				}
			}
			// step 8: generate update for user who has just been added
			for(pair<uint32_t, uint32_t> key : keys){
				send_key_update(user_id, key.first, key.second, false);
			}
			send_sql_update(user_id, update_new_user.append(update_old_user), true);
		} catch (...){
			/* Safe to return ok because the commit succeeded so the database is in a consistent state.
			 * The worst case is always assumed in SEkey APIs so the users are already in recovery. */
			return SEKEY_OK;
		}
		return SEKEY_OK;
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){ // transaction not active
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction(); // transaction still active
		}
	}
}
int sekey_delete_user_group(string& user_id, string& group_id){
	try{
		int rc;
		if(!is_admin){ /* this API is available only to the administrator */
			return SEKEY_ERR_AUTH;
		}
		if(user_id.empty() || user_id.length()>IDLEN || group_id.empty() || group_id.length()>IDLEN || !check_input(user_id, 0) || !check_input(group_id, 1)){
			return SEKEY_ERR_PARAMS;
		}
		string query;
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", deleted user " + user_id + " from group " + group_id;
		vector<string> users;
		statement sqlstmt;
		bool found = false;
		if(((rc = is_user_present(user_id)) != SEKEY_OK) || ((rc = is_group_present(group_id)) != SEKEY_OK)){ return rc; }
		if(sekey_recovery() != SEKEY_OK){ return SEKEY_UNCHANGED;	}
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_UNCHANGED; } /* start the transaction */
		// check if the user is in the group
		query = "SELECT COUNT(*) FROM UserGroup WHERE group_id = ?1 AND user_id = ?2;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
			return rollback_transaction();
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return rollback_transaction();
			}
			if(sqlite3_column_int64(sqlstmt.getstmt(), 0) == 1){
				found = true;
			}
		}
		if(!found){ // the user is not in this group
			return rollback_transaction();
		}
		// select all the users who need to be updated (excluding the user directly involved)
		query = "SELECT user_id FROM UserGroup WHERE group_id = ?1 AND user_id <> ?2;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), query) != SEKEY_OK)){
			return rollback_transaction();
		}
		if(sql_fill_vector(nullptr, query, &users) != SEKEY_OK){ // fill list of users involved by this change
			return rollback_transaction();
		}
		/* delete the user-group relation and adjust the counter of users for the group */
		query = "DELETE FROM UserGroup WHERE user_id = ?1 AND group_id = ?2;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return rollback_transaction();
		}
		query = "UPDATE Groups SET users_counter = users_counter - 1 WHERE group_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return rollback_transaction();
		}
		/* assume all users involved in this change will need db recovery, they will be removed from this list as soon as possible.
		 * this is done here because in the worst case we perform a recovery when we don't have to but at least we don't miss a recovery
		 * when we should do it. */
		vector<string> tempv;
		tempv.push_back(user_id);
		if((fill_recovery(users) != SEKEY_OK) || (fill_recovery(tempv) != SEKEY_OK)){
			return rollback_transaction();
		}
		if((rc=commit_transaction()) != SEKEY_OK){ return rc; } // transaction commit
		sekey_printlog(msg);
		/* distribute the update to the users */
		delete_user_from_group_iterator(users, user_id, group_id, true);
		req_delete_user_from_group(user_id, user_id, group_id, true);
		return SEKEY_OK;
		/* notice that all the code in the APIs which comes after the commit is always no throw because the commit worked so is reasonable to
		 * return OK to the caller. the code after the commit is only related to the update distribution, since the worst case scenario is
		 * always assumed, nothing has to be done in case of exception or generic error. the user is already considered to be in need of a
		 * database recovery, he will be removed from recovery only if the update is written correctly. in this way we use a whitelist approach
		 * instead of a blacklist, basically we handle only the case where everything goes well instead of many cases in which things go wrong.
		 * in case someone wanted to change this behavior in order to make the code after the commit able to throw, then the catch clause
		 * must be modified in order to check if the commit was done (along the active transaction check). be very careful about modifying the
		 * logic of recovery and update distribution because it is really complex and exceptions are sometimes handled in a bad way (meaning
		 * they are ignored when they should be propagated to higher levels) because the caller will not be able to handle the status of
		 * the underlying database since it is complex. */
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){ // transaction not active (commit already done or transaction not even started)
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction();
		}
	}
}
int sekey_delete_user(string& user_id){
	try{
		string query, sn;
		vector<string> users;
		statement sqlstmt;
		uint32_t algo = 0, k1 = 0, k2 = 0;
		bool found = false;
		int rc;
		if(!is_admin){ /* this API is available only to the administrator */
			return SEKEY_ERR_AUTH;
		}
		if(user_id.empty() || user_id.length()>IDLEN || !check_input(user_id, 0)){
			return SEKEY_ERR_PARAMS;
		}
		if((rc = is_user_present(user_id)) != SEKEY_OK){ return rc; }
		if(sekey_recovery() != SEKEY_OK){	return SEKEY_UNCHANGED;	}
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_UNCHANGED; }
		/* step 1: select all the users who have at least one group in common with the user to be deleted (except the user directly involved) */
		query = "SELECT DISTINCT us1.user_id FROM UserGroup us1 WHERE us1.group_id IN (SELECT us2.group_id "
				"FROM UserGroup us2 WHERE us2.user_id = ?1) AND us1.user_id <> ?2;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), query) != SEKEY_OK)){
				return rollback_transaction();
		}
		if(sql_fill_vector(nullptr, query, &users) != SEKEY_OK){
			return rollback_transaction();
		}
		/* step 2: retrieve the id of the keys to be deleted from the device */
		query = "SELECT k1, k2, key_algo, serial_number FROM Users WHERE user_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
			return rollback_transaction();
		}
		for(;;){
			if((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if(rc != SQLITE_ROW){
				return rollback_transaction();
			}
			k1 = get_u32(sqlstmt.getstmt(), 0);
			k2 = get_u32(sqlstmt.getstmt(), 1);
			algo = get_u32(sqlstmt.getstmt(), 2);
			sn = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 3);
			found = true;
		}
		if(found == false){
			return rollback_transaction();
		}
		// assume all users need recovery (must be done before removing the user entry from the db)
		vector<string> tempv;
		tempv.push_back(user_id);
		if((fill_recovery(users) != SEKEY_OK) || (fill_recovery(tempv) != SEKEY_OK)){
			return rollback_transaction();
		}
		/* step 3: decrement group counters, delete user from users and usergroup tables */
		query = "UPDATE Groups SET users_counter = users_counter - 1 WHERE group_id IN ("
						"SELECT t2.group_id FROM UserGroup t2 WHERE t2.user_id = ?1);";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return rollback_transaction();
		}
		query = "DELETE FROM Users WHERE user_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return rollback_transaction();
		}
		query = "DELETE FROM UserGroup WHERE user_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return rollback_transaction();
		}
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", deleted user " + user_id;
		if((rc=commit_transaction()) != SEKEY_OK){ return rc; } // transaction commit
		sekey_printlog(msg);
		/* generate update */
		delete_user_iterator(users, user_id, true);
		req_delete_user(user_id, algo, k1, sn, true, NORMAL);
		try { // delete the update key of this user from the flash of the admin's SEcube
			SEcube->L1SEkey_DeleteKey(k1);
			SEcube->L1SEkey_DeleteKey(k2);
			/* notice that we don't check the return value from the L1DeleteKey because we have already done the commit.
			 * k1 and k2 in the worst case will stay in the flash of the SEcube and will be deleted by the garbage collector. */
		} catch(...){ /* NOP: we don't have to do anything if this command fails. The garbage collector will take care of this. */ return SEKEY_OK; }
		return SEKEY_OK;
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){ // transaction not active (commit already done or transaction not even started)
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction();
		}
	}
}
int sekey_user_change_name(string& user_id, string& newname){
	try{
		string query;
		vector<string> users;
		int rc;
		statement sqlstmt;
		if(!is_admin){ /* this API is available only to the administrator */
			return SEKEY_ERR_AUTH;
		}
		if(user_id.empty() || newname.empty() || user_id.length()>IDLEN || newname.length()>NAMELEN || !check_input(user_id, 0)){
			return SEKEY_ERR_PARAMS; // check input
		}
		if((rc = is_user_present(user_id)) != SEKEY_OK){ return rc;	}
		if(sekey_recovery() != SEKEY_OK){ return SEKEY_UNCHANGED;	}
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_UNCHANGED; }
		/* step 1: retrieve the IDs of the users who have at least one group in common with the user whose name must be changed (including user whose name must be changed) */
		query = "SELECT DISTINCT us1.user_id FROM UserGroup us1 WHERE us1.group_id IN (SELECT us2.group_id FROM UserGroup us2 WHERE us2.user_id = ?1);";
		if(sql_fill_vector(&user_id, query, &users) != SEKEY_OK){
			return rollback_transaction();
		}
		/* step 2: change the name of the user on the administrator's database */
		query = "UPDATE Users SET username = ?1 WHERE user_id = ?2;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, newname.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE) ||
		   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), query) != SEKEY_OK)){
			return rollback_transaction();
		}
		// worst case assumption: put all users involved by this change in the list of users who need recovery
		if(fill_recovery(users) != SEKEY_OK){ // this is the last place where an exception could be thrown
			return rollback_transaction();
		}
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", changed name of user " + user_id;
		if((rc=commit_transaction()) != SEKEY_OK){ return rc; } // transaction commit
		sekey_printlog(msg);
		// step 3: write update queries in user files
		sql_update_iterator(users, query, true);
		return SEKEY_OK;
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){ // transaction not active (commit already done or transaction not even started)
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction();
		}
	}
}
int sekey_user_get_info_all(vector<se_user> *users){
	try{
		if(!user_allowed()){ return SEKEY_BLOCKED; }
		if(users == nullptr){
			return SEKEY_ERR_PARAMS;
		}
		int rc;
		statement sqlstmt;
		uint32_t k1, k2, algo, init_flag;
		string query, user_id, user_name, group_id, serialnumber, userpin, adminpin;
		int64_t cnt;
		query = "SELECT * FROM Users;";
		if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK){
			users->clear();
			return SEKEY_ERR;
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				users->clear();
				return SEKEY_ERR;
			}
			user_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			user_name = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
			serialnumber = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 2);
			userpin = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 3);
			adminpin = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 4);
			k1 = get_u32(sqlstmt.getstmt(), 5);
			k2 = get_u32(sqlstmt.getstmt(), 6);
			algo = get_u32(sqlstmt.getstmt(), 7);
			init_flag = get_u32(sqlstmt.getstmt(), 8);
			cnt = sqlite3_column_int64(sqlstmt.getstmt(), 9);
			se_user current_user(user_id, user_name, serialnumber, userpin, adminpin, k1, k2, algo, init_flag, cnt);
			users->push_back(current_user);
		}
		for(vector<se_user>::iterator it = users->begin(); it != users->end(); ++it){
			query = "SELECT group_id FROM UserGroup WHERE user_id = ?1;";
			if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 1, it->get_id().c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
				users->clear();
				return SEKEY_ERR;
			}
			for(;;){
				if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
					break;
				}
				if (rc != SQLITE_ROW) {
					users->clear();
					return SEKEY_ERR;
				}
				group_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
				it->add_group(group_id);
			}
		}
		return SEKEY_OK;
	} catch(...){
		users->clear();
		return SEKEY_ERR;
	}
}
int sekey_user_get_info(string& user_id, se_user *user){
	try{
		if(!user_allowed()){ return SEKEY_BLOCKED; }
		if(user == nullptr || user_id.empty() || user_id.length()>IDLEN || !check_input(user_id, 0)){
			return SEKEY_ERR_PARAMS;
		}
		int rc;
		if((rc = is_user_present(user_id)) != SEKEY_OK){ return rc; }
		uint32_t k1, k2, algo, init_flag;
		string query, user_name, group_id, serialnumber, userpin, adminpin;
		int64_t cnt;
		statement sqlstmt;
		query = "SELECT * FROM Users WHERE user_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)) != SQLITE_OK ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
			return SEKEY_ERR;
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return SEKEY_ERR;
			}
			user_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			user_name = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
			serialnumber = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 2);
			userpin = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 3);
			adminpin = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 4);
			k1 = get_u32(sqlstmt.getstmt(), 5);
			k2 = get_u32(sqlstmt.getstmt(), 6);
			algo = get_u32(sqlstmt.getstmt(), 7);
			init_flag = get_u32(sqlstmt.getstmt(), 8);
			cnt = sqlite3_column_int64(sqlstmt.getstmt(), 9);
			se_user current_user(user_id, user_name, serialnumber, userpin, adminpin, k1, k2, algo, init_flag, cnt);
			*user = current_user;
		}
		query = "SELECT group_id FROM UserGroup WHERE user_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
			return SEKEY_ERR;
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return SEKEY_ERR;
			}
			group_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			user->add_group(group_id);
		}
		return SEKEY_OK;
	} catch(...){
		return SEKEY_ERR;
	}
}

/* KEY RELATED APIs */
int sekey_get_key_ids(vector<string>& ids){
	try{
		if(!user_allowed()){ return SEKEY_BLOCKED; }
		string query = "SELECT key_id FROM SeKeys;";
		string id;
		statement sqlstmt;
		int rc;
		if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK){
			return SEKEY_ERR;
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return SEKEY_ERR;
			}
			id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			ids.push_back(id);
		}
		return SEKEY_OK;
	} catch (...) {
		return SEKEY_ERR;
	}
}
int sekey_add_key(string& key_id, string& key_name, string& group_id, uint32_t cryptoperiod, se_key_type keytype){
	try{
		string query, update_query = "";
		statement sqlstmt;
		vector<string> users;
		uint32_t algo = 0;
		string ksub = key_id.substr(1);
		int rc;
		if(!is_admin){ /* this API is available only to the administrator */
			return SEKEY_ERR_AUTH;
		}
		if(key_id.empty() || key_name.empty() || group_id.empty() || key_id.length()>IDLEN || key_name.length()>NAMELEN ||
		   group_id.length()>IDLEN || !check_input(group_id, 1) || (stoul_wrap(ksub) < L1Key::Id::SEKEY_ID_BEGIN) ||
		   (stoul_wrap(ksub) > L1Key::Id::SEKEY_ID_END) || keytype<=se_key_type::typemin || keytype>=se_key_type::typemax || !check_input(key_id, 2)){
			return SEKEY_ERR_PARAMS;
		}
		if(keytype != se_key_type::symmetric_data_encryption){
			return SEKEY_UNSUPPORTED;
		}
		if((rc = is_group_present(group_id)) != SEKEY_OK){ return rc; }
		if(sekey_recovery() != SEKEY_OK){ return SEKEY_UNCHANGED;	}
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_UNCHANGED; }
		/* step 1: retrieve the algorithm policy of the owner */
		query = "SELECT algorithm FROM Groups WHERE group_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
			return rollback_transaction();
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return rollback_transaction();
			}
			algo = get_u32(sqlstmt.getstmt(), 0);
		}
		if(!algovalid(algo) || (algolen(algo) == 0)){
			return rollback_transaction();
		}
		/* step 2: retrieve the list of users who belong to the group that owns the key */
		query = "SELECT DISTINCT user_id FROM UserGroup WHERE group_id = ?1;";
		if(sql_fill_vector(&group_id, query, &users) != 0){
			return rollback_transaction();
		}
		/* step 3: insert the key into the database and increment the key counter of the key owner */
		query = "INSERT INTO SeKeys(key_id, key_name, key_owner, status, algorithm, key_length, generation, activation, expiration, cryptoperiod, deactivation, type, compromise, destruction, suspension) "
				"VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15);";
		string s_;
		if( (sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
			(sqlite3_bind_text(sqlstmt.getstmt(), 1, key_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			(sqlite3_bind_text(sqlstmt.getstmt(), 2, key_name.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			(sqlite3_bind_text(sqlstmt.getstmt(), 3, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 4, (uint32_t)se_key_status::preactive) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 5, algo) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 6, algolen(algo)) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 7, sekey_gettime()) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 8, 0) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 9, 0) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 10, cryptoperiod) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 11, 0) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 12, (uint32_t)keytype) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 13, 0) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 14, 0) != SQLITE_OK) ||
			(sqlite3_bind_int64(sqlstmt.getstmt(), 15, 0) != SQLITE_OK) ||
			((rc = sqlite3_step(sqlstmt.getstmt())) != SQLITE_DONE) ||
			(sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), s_) != SEKEY_OK)){
			if(rollback_transaction() == SEKEY_RESTART){ return SEKEY_RESTART; }
			if(rc == SQLITE_CONSTRAINT_PRIMARYKEY){
				return SEKEY_KEY_DUP;
			}
			return SEKEY_UNCHANGED;
		}
		update_query.append(s_);
		query =	"UPDATE Groups SET keys_counter = keys_counter + 1 WHERE group_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE) ||
		   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), s_) != SEKEY_OK)){
			return rollback_transaction();
		}
		update_query.append(s_);
		ksub = key_id.substr(1);
		uint32_t tmpid = stoul_wrap(ksub);
		if(!SEcube->L1SEkey_InsertKey(tmpid, algolen(algo), 0, nullptr)){
			return rollback_transaction();
		}
		if(fill_recovery(users) != SEKEY_OK){
			return rollback_transaction();
		}
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", added key " + key_id;
		if((rc=commit_transaction()) != SEKEY_OK){ return rc; } // transaction commit
		sekey_printlog(msg);
		/* step 4: deliver updates */
		sql_update_iterator(users, update_query, false);
		key_update_iterator(users, tmpid, algolen(algo), true);
		return SEKEY_OK;
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){ // transaction not active (commit already done or transaction not even started)
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction();
		}
	}
}
int sekey_key_change_name(string& key_id, string& key_name){
	try{
		string query;
		statement sqlstmt;
		vector<string> users;
		int rc;
		if(!is_admin){ /* this API is available only to the administrator */
			return SEKEY_ERR_AUTH;
		}
		if(key_id.empty() || key_name.empty() || key_id.length()>IDLEN || key_name.length()>NAMELEN || !check_input(key_id, 2)){
			return SEKEY_ERR_PARAMS;
		}
		if((rc = is_key_present(key_id)) != SEKEY_OK){ return rc; }
		if(sekey_recovery() != SEKEY_OK){	return SEKEY_UNCHANGED; }
		if((rc=sekey_check_expired_keys()) != SEKEY_OK){ return rc; }
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_UNCHANGED; }
		/* step 1: retrieve the list of users who belong to the group that owns the key */
		query = "SELECT user_id FROM UserGroup WHERE group_id IN (SELECT key_owner FROM SeKeys WHERE key_id = ?1);";
		if(sql_fill_vector(&key_id, query, &users) != 0){
			return rollback_transaction();
		}
		/* step 2: update key name */
		query = "UPDATE SeKeys SET key_name = ?1 WHERE key_id = ?2;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)!=SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, key_name.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, key_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt())!=SQLITE_DONE) ||
		   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), query) != SEKEY_OK)){
			return rollback_transaction();
		}
		if(fill_recovery(users) != SEKEY_OK){
			return rollback_transaction();
		}
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", changed name of key " + key_id;
		if((rc=commit_transaction()) != SEKEY_OK){ return rc; } // transaction commit
		sekey_printlog(msg);
		/* step 3: generate updates */
		sql_update_iterator(users, query, true);
		return SEKEY_OK;
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){ // transaction not active (commit already done or transaction not even started)
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction();
		}
	}
}
int sekey_key_change_status(string& key_id, se_key_status status){
	try{
		string query, group_id;
		statement sqlstmt;
		vector<string> users;
		se_key_status current_status = se_key_status::statusmin;
		time_t exptime = 0;
		int rc;
		if(!is_admin){ /* this API is available only to the administrator */
			return SEKEY_ERR_AUTH;
		}
		if(status == se_key_status::active || status >= se_key_status::statusmax || status <= se_key_status::statusmin
				|| key_id.empty() || key_id.length()>IDLEN || !check_input(key_id, 2) || status == se_key_status::preactive){
			return SEKEY_ERR_PARAMS;
		}
		if((rc=is_key_present(key_id)) != SEKEY_OK){ return rc;	}
		if(sekey_recovery() != SEKEY_OK){ return SEKEY_UNCHANGED;	}
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_UNCHANGED; }
		/* step 1: retrieve owner id (group id), status and expiration time of the key */
		query = "SELECT key_owner, status, expiration FROM SeKeys WHERE key_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, key_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
			return rollback_transaction();
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){ // the query ended correctly
				break;
			}
			if (rc != SQLITE_ROW) { // query returned wrong value
				return rollback_transaction();
			}
			group_id.assign(sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0));
			current_status = (se_key_status)get_u32(sqlstmt.getstmt(), 1);
			exptime = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 2);
		}
		if(current_status <= se_key_status::statusmin || current_status >= se_key_status::statusmax || check_key_transition_validity(current_status, status) != 0){
			if((rc = rollback_transaction()) == SEKEY_UNCHANGED){
				return SEKEY_ERR; // to signal that the status is not compatible (i.e. activate a key which is deactivated)
			} else {
				return rc;
			}
		}
		/* step 2: retrieve list of users interested in this change */
		query = "SELECT user_id FROM UserGroup WHERE group_id = ?1;";
		if(sql_fill_vector(&group_id, query, &users) != 0){
			return rollback_transaction();
		}
		/* step 3: update key status and set the time of the status change if needed */
		switch((uint32_t)status){
			case (uint32_t)se_key_status::deactivated:
				if(exptime == 0){ // in case a key is deactivated without ever being active
					exptime = sekey_gettime();
				}
				query = "UPDATE SeKeys SET status = " + to_string((uint32_t)se_key_status::deactivated) + ", deactivation = " + to_string((uint32_t)exptime) + " WHERE key_id = ?1;";
				break;
			case (uint32_t)se_key_status::compromised:
				query = "UPDATE SeKeys SET status = " + to_string((uint32_t)se_key_status::compromised) + ", compromise = " + to_string((uint32_t)sekey_gettime()) + " WHERE key_id = ?1;";
				break;
			case (uint32_t)se_key_status::destroyed:
				query = "UPDATE SeKeys SET status = " + to_string((uint32_t)se_key_status::destroyed) + ", destruction = " + to_string((uint32_t)sekey_gettime()) + " WHERE key_id = ?1;";
				break;
			case (uint32_t)se_key_status::suspended:
				query = "UPDATE SeKeys SET status = " + to_string((uint32_t)se_key_status::suspended) + ", suspension = " + to_string((uint32_t)sekey_gettime()) + " WHERE key_id = ?1;";
				break;
			default:
				return rollback_transaction();
		}
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, key_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE) ||
		   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), query) != SEKEY_OK)){
			return rollback_transaction();
		}
		if(fill_recovery(users) != SEKEY_OK){
			return rollback_transaction();
		}
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", changed status of key " + key_id;
		if((rc=commit_transaction()) != SEKEY_OK){ return rc; } // transaction commit
		sekey_printlog(msg);
		/* step 4: update distribution */
		sql_update_iterator(users, query, true);
		/* step 5: delete the key from the SEcube if needed */
		if(status == se_key_status::destroyed){
			string ksub = key_id.substr(1);
			uint32_t kid = stoul_wrap(ksub);
			SEcube->L1SEkey_DeleteKey(kid); // don't check if deletion works or not, worst case is we issue the deletion again in the garbage collector
		}
		return SEKEY_OK;
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){ // transaction not active (commit already done or transaction not even started)
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction();
		}
	}
}
int sekey_activate_key(string& key_id){
	try{
		string query, group_id;
		statement sqlstmt;
		se_key_status current_status = se_key_status::statusmin;
		time_t act_time = 0, exp_time = 0, cryptoperiod = 0;
		vector<string> users;
		bool found = false;
		int rc;
		if(!is_admin){ /* this API is available only to the administrator */
			return SEKEY_ERR_AUTH;
		}
		if(key_id.empty() || key_id.length()>IDLEN || !check_input(key_id, 2)){
			return SEKEY_ERR_PARAMS;
		}
		if((rc=is_key_present(key_id)) != SEKEY_OK){ return rc; }
		if(sekey_recovery() != SEKEY_OK){ return SEKEY_UNCHANGED; }
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_UNCHANGED; }
		/* step 1: retrieve owner and status of the key */
		query = "SELECT key_owner, status, activation, cryptoperiod FROM SeKeys WHERE key_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, key_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
			return rollback_transaction();
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return rollback_transaction();
			}
			group_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			current_status = (se_key_status)get_u32(sqlstmt.getstmt(), 1);
			act_time = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 2);
			cryptoperiod = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 3);
			found = true;
		}
		if(current_status >= se_key_status::statusmax || current_status <= se_key_status::statusmin ||
				check_key_transition_validity(current_status, se_key_status::active) != 0 || found == false){
			if((rc = rollback_transaction()) == SEKEY_UNCHANGED){
				return SEKEY_ERR; // to signal that the status is not compatible (i.e. activate a key which is deactivated)
			} else {
				return rc;
			}
		}
		/* step 2: retrieve the default cryptoperiod of the group that owns the key (if first activation and the key hasn't got a specific cryptoperiod) */
		if((act_time==0) && (cryptoperiod==0)){
			query = "SELECT keys_liveness FROM Groups WHERE group_id = ?1;";
			found = false;
			if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
				return rollback_transaction();
			}
			for(;;){
				if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
					break;
				}
				if (rc != SQLITE_ROW) {
					return rollback_transaction();
				}
				cryptoperiod = get_u32(sqlstmt.getstmt(), 0);
				found = true;
			}
			if(found == false || cryptoperiod == 0){
				return rollback_transaction();
			}
		}
		/* step 3: retrieve list of users interested in this change */
		query = "SELECT user_id FROM UserGroup WHERE group_id = ?1;";
		if(sql_fill_vector(&group_id, query, &users) != 0){
			return rollback_transaction();
		}
		/* step 4: compute the values of activation and expiration (if first activation), then update the database */
		if(act_time == 0){
			act_time = sekey_gettime();
			if(act_time < 0){
				return rollback_transaction();
			}
			if(sizeof(time_t) < 4){ // in theory this is not necessary
					return rollback_transaction();
			}
			if(sizeof(time_t) == 4 && is_signed<time_t>::value){ // this is in case of Y2038 bug
				int64_t tempval = (int64_t)act_time + (int64_t)cryptoperiod;
				if(tempval > INT32_MAX){
					return rollback_transaction();
				}
			}
			if(sizeof(time_t) == 4 && !is_signed<time_t>::value){ // this is in case of 32 bit unsigned time_t
				int64_t tempval = (int64_t)act_time + (int64_t)cryptoperiod;
				if(tempval > UINT32_MAX){
					return rollback_transaction();
				}
			}
			exp_time = act_time + cryptoperiod;
			query = "UPDATE SeKeys SET status = " + to_string((uint32_t)se_key_status::active) + ", activation = " + to_string(act_time) + ", expiration = " + to_string(exp_time) + " WHERE key_id = ?1;";
		} else {
			query = "UPDATE SeKeys SET status = " + to_string((uint32_t)se_key_status::active) + " WHERE key_id = ?1;";
		}
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, key_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE) ||
		   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), query) != SEKEY_OK)){
			return rollback_transaction();
		}
		if(fill_recovery(users) != SEKEY_OK){
			return rollback_transaction();
		}
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", activated key " + key_id;
		if((rc=commit_transaction()) != SEKEY_OK){ return rc; } // transaction commit
		sekey_printlog(msg);
		/* step 5: update distribution */
		sql_update_iterator(users, query, true);
		return SEKEY_OK;
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){ // transaction not active (commit already done or transaction not even started)
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction();
		}
	}
}
int sekey_check_expired_keys(){
	try{
		bool failed = false;
		// only active and suspended keys can be set to deactivated status (transition to deactivated is not allowed from preactive, compromised and destroyed)
		string query = "SELECT key_id, expiration FROM SeKeys WHERE status = " + to_string((uint32_t)se_key_status::active) + " OR status = " + to_string((uint32_t)se_key_status::suspended) + ";";
		statement sqlstmt;
		time_t exp = 0;
		vector<string> to_deactivate;
		int rc;
		if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK){
			return SEKEY_ERR;
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return SEKEY_ERR;
			}
			string key_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			exp = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 1);
			/* check if the key is expired or is going to expire in the next 60 seconds. in this case the key must be deactivated and can't be used for encryption.
			 * the 60 seconds are added simply because there's a delay from the moment we check if a key is still valid and the moment we actually use it for encryption.
			 * in particular we may retrieve the key and use it only some time later, in case of a big file to encrypt it may happen that the key expires before we finish
			 * encrypting the file. so we keep this safety range to guard against this possibility, in any case the application using SEkey should always keep the lag
			 * between the key retrieval and the key usage as small as possible. notice that, once a key is chosen, it must be used to encrypt the entire file and the
			 * application can't change the key to encrypt some sectors just because the initial key expired in the meantime. final note: the value of 60 seconds can
			 * be tweaked to meet particular requirements, in case of a huge database with thousands of keys this value may be higher to consider the delay in reading
			 * the entire database. */
			time_t tn = sekey_gettime() + 60;
			if((exp <= tn) && (exp != 0)){
				to_deactivate.push_back(key_id);
			}
		}
		for(string k : to_deactivate){
			if(is_admin){ // if the administrator is executing this function
				rc = sekey_key_change_status(k, se_key_status::deactivated);
				if(rc != SEKEY_OK){
					failed = true;
				}
			} else { // if a user is executing this function simply deactivate the key
				query = "UPDATE SeKeys SET status = " + to_string((uint32_t)se_key_status::deactivated) + ", deactivation = " + to_string((uint32_t)exp) + " WHERE key_id = '" + k + "';";
				rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);
				if(rc != SEKEY_OK){
					failed = true;
				}
				string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", key " + k + " deactivated";
				sekey_printlog(msg);
			}
		}
		if(failed){
			return SEKEY_ERR;
		} else {
			return SEKEY_OK;
		}
	} catch (...){
		return SEKEY_ERR;
	}
}
int sekey_key_get_info(string& key_id, se_key *key){
	try{
		if(!user_allowed()){ return SEKEY_BLOCKED; }
		if(key_id.empty() || key_id.length()>IDLEN || key==nullptr || !check_input(key_id, 2)){
			return SEKEY_ERR_PARAMS;
		}
		sekey_check_expired_keys(); // deactivate expired keys (just a maintenance routine...)
		int rc;
		if((rc=is_key_present(key_id)) != SEKEY_OK){ return rc;	}
		string query, key_name, key_owner;
		statement sqlstmt;
		query = "SELECT * FROM SeKeys WHERE key_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, key_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
			return SEKEY_ERR;
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return SEKEY_ERR;
			}
			// skip 0 (key_id) because it is not required
			key_name = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
			key_owner = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 2);
			se_key_status key_status = (se_key_status)get_u32(sqlstmt.getstmt(), 3);
			uint32_t key_algo = get_u32(sqlstmt.getstmt(), 4);
			uint32_t key_length = get_u32(sqlstmt.getstmt(), 5);
			time_t gen = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 6);
			time_t act = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 7);
			time_t exp = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 8);
			time_t crypto = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 9);
			time_t deactivation = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 10);
			se_key_type key_type = (se_key_type)get_u32(sqlstmt.getstmt(), 11);
			time_t compr = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 12);
			time_t destr = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 13);
			time_t susp = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 14);
			se_key k(key_id, key_name, key_owner, key_status, key_algo, key_length, gen, act, exp, crypto, deactivation, key_type, compr, destr, susp);
			*key = k;
		}
		return SEKEY_OK;
	} catch(...){
		return SEKEY_ERR;
	}
}
int sekey_key_get_info_all(vector<se_key> *keys){
	try{
		if(!user_allowed()){ return SEKEY_BLOCKED; }
		if(keys == nullptr){
			return SEKEY_ERR_PARAMS;
		}
		sekey_check_expired_keys(); // deactivate expired keys (just a maintenance routine...)
		statement sqlstmt;
		string key_id, key_name, key_owner, query = "SELECT * FROM SeKeys;";
		if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK){
			return SEKEY_ERR;
		}
		for(;;){
			int rc = sqlite3_step(sqlstmt.getstmt());
			if (rc == SQLITE_DONE){ // the query ended correctly
				break;
			}
			if (rc != SQLITE_ROW) { // query returned wrong value
				return SEKEY_ERR;
			}
			key_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			key_name = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
			key_owner = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 2);
			se_key_status key_status = (se_key_status)get_u32(sqlstmt.getstmt(), 3);
			uint32_t key_algo = get_u32(sqlstmt.getstmt(), 4);
			uint32_t key_length = get_u32(sqlstmt.getstmt(), 5);
			time_t gen = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 6);
			time_t act = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 7);
			time_t exp = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 8);
			time_t crypto = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 9);
			time_t deactivation = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 10);
			se_key_type key_type = (se_key_type)get_u32(sqlstmt.getstmt(), 11);
			time_t compr = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 12);
			time_t destr = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 13);
			time_t susp = (time_t)sqlite3_column_int64(sqlstmt.getstmt(), 14);
			se_key k(key_id, key_name, key_owner, key_status, key_algo, key_length, gen, act, exp, crypto, deactivation, key_type, compr, destr, susp);
			keys->push_back(k);
		}
		return SEKEY_OK;
	} catch(...){
		return SEKEY_ERR;
	}
}

/* GROUP RELATED APIs */
int sekey_get_group_ids(vector<string>& ids){
	try{
		if(!user_allowed()){ return SEKEY_BLOCKED; }
		string query = "SELECT group_id FROM Groups;";
		string id;
		statement sqlstmt;
		int rc;
		if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK){
			return SEKEY_ERR;
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return SEKEY_ERR;
			}
			id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			ids.push_back(id);
		}
		return SEKEY_OK;
	} catch (...) {
		return SEKEY_ERR;
	}
}
int sekey_add_group(string& group_id, string& group_name, group_policy policy){
	try{
		statement sqlstmt;
		int rc = 0;
		if(!is_admin){ /* this API is available only to the administrator */
			return SEKEY_ERR_AUTH;
		}
		if(group_id.empty() || group_name.empty() || group_id.length()>IDLEN || group_name.length()>NAMELEN || policy.isvalid()==false || !check_input(group_id, 1)){
			return SEKEY_ERR_PARAMS;
		}
		if((rc = is_group_present(group_id)) == SEKEY_OK){ return SEKEY_GROUP_DUP;	}
		if(sekey_recovery() != SEKEY_OK){ return SEKEY_UNCHANGED;	}
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_UNCHANGED; }
		string query = "INSERT INTO Groups(group_id, group_name, users_counter, keys_counter, max_keys, algorithm, keys_liveness) VALUES(?1, ?2, ?3, ?4, ?5, ?6, ?7);";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)	||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, group_name.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_int64(sqlstmt.getstmt(), 3, 0) != SQLITE_OK) ||
		   (sqlite3_bind_int64(sqlstmt.getstmt(), 4, 0) != SQLITE_OK) ||
		   (sqlite3_bind_int64(sqlstmt.getstmt(), 5, policy.get_max_keys()) != SQLITE_OK) ||
		   (sqlite3_bind_int64(sqlstmt.getstmt(), 6, policy.get_algorithm()) != SQLITE_OK) ||
		   (sqlite3_bind_int64(sqlstmt.getstmt(), 7, policy.get_default_cryptoperiod()) != SQLITE_OK) ||
		   (rc = sqlite3_step(sqlstmt.getstmt())) != SQLITE_DONE){
			   if(rollback_transaction() == SEKEY_RESTART){ return SEKEY_RESTART; }
			   if(rc == SQLITE_CONSTRAINT_PRIMARYKEY){
				   return SEKEY_GROUP_DUP;
			   }
			   return SEKEY_UNCHANGED;
		}
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", added group " + group_id;
		rc = commit_transaction();
		if(rc == SEKEY_OK){
			sekey_printlog(msg);
		}
		return rc;
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){ // transaction not active (commit already done or transaction not even started)
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction();
		}
	}
}
int sekey_delete_group(string& group_id){
	try{
		string query;
		vector<string> users;
		statement sqlstmt;
		int rc;
		if(!is_admin){ /* this API is available only to the administrator */
			return SEKEY_ERR_AUTH;
		}
		if(group_id.empty() || group_id.length()>IDLEN || !check_input(group_id, 1)){
			return SEKEY_ERR_PARAMS;
		}
		if((rc=is_group_present(group_id)) != SEKEY_OK){ return rc;	}
		if(sekey_recovery() != SEKEY_OK){	return SEKEY_UNCHANGED;	}
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_UNCHANGED; }
		/* step 1: retrieve list of users belonging to the group to be deleted. */
		query = "SELECT DISTINCT user_id FROM UserGroup WHERE group_id = ?1;";
		if(sql_fill_vector(&group_id, query, &users) != SEKEY_OK){
			return rollback_transaction();
		}
		/* step 2: delete the group from the group table and from the user table. */
		query = "DELETE FROM Groups WHERE group_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return rollback_transaction();
		}
		query = "DELETE FROM UserGroup WHERE group_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return rollback_transaction();
		}
		/* step 3: change the owner of all keys of the deleted group to zombie */
		query = "UPDATE SeKeys SET key_owner = 'zombie' WHERE key_owner = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return rollback_transaction();
		}
		/* step 4: deactivate all keys of the deleted group that are not yet in deactivated, compromised or destroyed status */
		time_t currtime = sekey_gettime();
		query = "UPDATE SeKeys SET status = " + to_string((uint32_t)se_key_status::deactivated) + ", deactivation = " + to_string((uint64_t)currtime) + " WHERE key_owner = ?1 AND status <> " + to_string((uint32_t)se_key_status::deactivated) +
				" AND status <> " + to_string((uint32_t)se_key_status::compromised) + " AND status <> " + to_string((uint32_t)se_key_status::destroyed) + ";";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return rollback_transaction();
		}
		if(fill_recovery(users) != SEKEY_OK){
			return rollback_transaction();
		}
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", deleted group " + group_id;
		if((rc=commit_transaction()) != SEKEY_OK){ return rc; } // transaction commit
		sekey_printlog(msg);
		/* step 5: distribute updates */
		delete_group_iterator(users, group_id, true);
		return SEKEY_OK;
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){ // transaction not active (commit already done or transaction not even started)
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction();
		}
	}
}
int sekey_group_change_name(string& group_id, string& newname){
	try{
		string query;
		vector<string> users;
		statement sqlstmt;
		int rc;
		if(!is_admin){ /* this API is available only to the administrator */
			return SEKEY_ERR_AUTH;
		}
		if(group_id.empty() || group_id.length()>IDLEN || newname.empty() || newname.length()>NAMELEN || !check_input(group_id, 1)){
			return SEKEY_ERR_PARAMS;
		}
		if((rc=is_group_present(group_id)) != SEKEY_OK){ return rc;	}
		if(sekey_recovery() != SEKEY_OK){ return SEKEY_UNCHANGED;	}
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_UNCHANGED; }
		/* step 1: select all the users who belong to this group */
		query = "SELECT user_id FROM UserGroup WHERE group_id = ?1;";
		if(sql_fill_vector(&group_id, query, &users) != 0){
			return rollback_transaction();
		}
		/* step 2: update the name of the group */
		query = "UPDATE Groups SET group_name = ?1 WHERE group_id = ?2;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, newname.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE) ||
		   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), query) != SEKEY_OK)){
			return rollback_transaction();
		}
		if(fill_recovery(users) != SEKEY_OK){
			return rollback_transaction();
		}
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", changed name of group " + group_id;
		if((rc=commit_transaction()) != SEKEY_OK){ return rc; } // transaction commit
		sekey_printlog(msg);
		/* step 3: update distribution */
		sql_update_iterator(users, query, true);
		return SEKEY_OK;
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){ // transaction not active (commit already done or transaction not even started)
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction();
		}
	}
}
int sekey_group_change_max_keys(string& group_id, uint32_t maxkeys){
	try{
		string query;
		statement sqlstmt;
		uint32_t current_keys = UINT32_MAX;
		vector<string> users;
		int rc;
		if(!is_admin){ /* this API is available only to the administrator */
			return SEKEY_ERR_AUTH;
		}
		if(group_id.empty() || group_id.length()>IDLEN || maxkeys == 0 || !check_input(group_id, 1)){
			return SEKEY_ERR_PARAMS;
		}
		if((rc=is_group_present(group_id)) != SEKEY_OK){ return rc;	}
		if(sekey_recovery() != SEKEY_OK){ return SEKEY_UNCHANGED;	}
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_UNCHANGED; }
		/* step 1: retrieve current number of keys */
		query = "SELECT keys_counter FROM Groups WHERE group_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
			return rollback_transaction();
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return rollback_transaction();
			}
			current_keys = get_u32(sqlstmt.getstmt(), 0);
		}
		if(maxkeys < current_keys){
			return rollback_transaction();
		}
		/* step 2: select all the users who belong to that group */
		query = "SELECT user_id FROM UserGroup WHERE group_id = ?1;";
		if(sql_fill_vector(&group_id, query, &users) != SEKEY_OK){
			return rollback_transaction();
		}
		/* step 3: update maximum number of keys */
		query = "UPDATE Groups SET max_keys = ?1 WHERE group_id = ?2;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_int64(sqlstmt.getstmt(), 1, maxkeys) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE) ||
		   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), query) != SEKEY_OK)){
			return rollback_transaction();
		}
		if(fill_recovery(users) != SEKEY_OK){
			return rollback_transaction();
		}
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", changed max keys policy of group " + group_id;
		if((rc=commit_transaction()) != SEKEY_OK){ return rc; } // transaction commit
		sekey_printlog(msg);
		/* step 4: distribute the update */
		sql_update_iterator(users, query, true);
		return SEKEY_OK;
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){ // transaction not active (commit already done or transaction not even started)
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction();
		}
	}
}
int sekey_group_change_default_cryptoperiod(string& group_id, uint32_t cryptoperiod){
	try{
		vector<string> users;
		statement sqlstmt;
		int rc;
		if(!is_admin){ /* this API is available only to the administrator */
			return SEKEY_ERR_AUTH;
		}
		if(group_id.empty() || group_id.length()>IDLEN || cryptoperiod==0 || !check_input(group_id, 1)){
			return SEKEY_ERR_PARAMS;
		}
		if((rc=is_group_present(group_id)) != SEKEY_OK){ return rc;	}
		if(sekey_recovery() != SEKEY_OK){ return SEKEY_UNCHANGED;	}
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_UNCHANGED; }
		/* step 1: retrieve list of users belonging to the group */
		string query = "SELECT user_id FROM UserGroup WHERE group_id = ?1;";
		if(sql_fill_vector(&group_id, query, &users) != SEKEY_OK){
			return rollback_transaction();
		}
		/* step 2: update database */
		query = "UPDATE Groups SET keys_liveness = ?1 WHERE group_id = ?2;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)!=SQLITE_OK) ||
		   (sqlite3_bind_int64(sqlstmt.getstmt(), 1, cryptoperiod)!=SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, group_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt())!=SQLITE_DONE) ||
		   (sqlite3_expanded_sql_wrapper(sqlstmt.getstmt(), query) != SEKEY_OK)){
			return rollback_transaction();
		}
		if(fill_recovery(users) != SEKEY_OK){
			return rollback_transaction();
		}
		string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", changed default cryptoperiod of group " + group_id;
		if((rc=commit_transaction()) != SEKEY_OK){ return rc; } // transaction commit
		sekey_printlog(msg);
		/* step 3: update users' database */
		sql_update_iterator(users, query, true);
		return SEKEY_OK;
	} catch(...){
		if(sqlite3_get_autocommit(db) != 0){ // transaction not active (commit already done or transaction not even started)
			return SEKEY_UNCHANGED;
		} else {
			return rollback_transaction();
		}
	}
}
int sekey_group_get_info(string& group_id, se_group *group){
	try{
		int rc;
		if(!user_allowed()){ return SEKEY_BLOCKED; } // check if the user is blocked
		if(group_id.empty() || group_id.length()>IDLEN || group==nullptr || !check_input(group_id, 1)){
			return SEKEY_ERR_PARAMS;
		}
		if((rc=is_group_present(group_id)) != SEKEY_OK){ return rc; }
		string query, temp_id, temp_name;
		statement sqlstmt;
		query = "SELECT * FROM Groups WHERE group_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
			return SEKEY_ERR;
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return SEKEY_ERR;
			}
			temp_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			temp_name = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
			group->set_id(temp_id);
			group->set_name(temp_name);
			group->set_users_counter(get_u32(sqlstmt.getstmt(), 2));
			group->set_keys_counter(get_u32(sqlstmt.getstmt(), 3));
			group->set_keys_maxnumber(get_u32(sqlstmt.getstmt(), 4));
			group->set_keys_algorithm(get_u32(sqlstmt.getstmt(), 5));
			group->set_keys_cryptoperiod(get_u32(sqlstmt.getstmt(), 6));
		}

		return SEKEY_OK;
	} catch(...){
		return SEKEY_ERR;
	}
}
int sekey_group_get_info_all(vector<se_group> *groups){
	try{
		if(!user_allowed()){ return SEKEY_BLOCKED; }
		string query = "SELECT * FROM Groups;", temp_id, temp_name;
		statement sqlstmt;
		int rc;
		if(groups == nullptr){
			return SEKEY_ERR_PARAMS;
		}
		if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK){
			return SEKEY_ERR;
		}
		for(;;){
			if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if (rc != SQLITE_ROW) {
				return SEKEY_ERR;
			}
			se_group group;
			temp_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			temp_name = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 1);
			group.set_id(temp_id);
			group.set_name(temp_name);
			group.set_users_counter(get_u32(sqlstmt.getstmt(), 2));
			group.set_keys_counter(get_u32(sqlstmt.getstmt(), 3));
			group.set_keys_maxnumber(get_u32(sqlstmt.getstmt(), 4));
			group.set_keys_algorithm(get_u32(sqlstmt.getstmt(), 5));
			group.set_keys_cryptoperiod(get_u32(sqlstmt.getstmt(), 6));
			groups->push_back(group);
		}
		return SEKEY_OK;
	} catch(...){
		return SEKEY_ERR;
	}
}

/* APIs that allow further SEkey customization */
time_t sekey_gettime(){
	time_t now = time(nullptr);
	return now;
	/* each developer should modify this implementation in order to meet the security domain requirements (i.e. using an authoritative timestamp source) */
}

/* FUNCTIONS TO EXECUTE THE CONTENT OF THE UPDATE FILE */
int usr_delete_user_from_group(char *buffer){
	if(is_admin){ return SEKEY_ERR_AUTH; }
	if(buffer == nullptr){ return SEKEY_ERR; }
	string query;
	statement sqlstmt;
	int rc;
	bool this_user = false;
	uint16_t uid_len, gid_len, offset = 0;
	memcpy(&uid_len, buffer+offset, 2);
	offset+=2;
	string uid(buffer+offset, uid_len);
	offset+=uid_len;
	memcpy(&gid_len, buffer+offset, 2);
	offset+=2;
	string gid(buffer+offset, gid_len);
	if(uid.compare(currentuser.userid) == 0){
		this_user = true;
	}
	string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", deleted user " + uid + " from group " + gid + "\n";
	logs.append(msg);
	if(this_user){
		/* physically delete the keys of the group from the SEcube internal memory */
		vector<uint32_t> keys_to_delete;
		query = "SELECT key_id FROM SeKeys WHERE key_owner = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, gid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
			return SEKEY_ERR;
		}
		for(;;){
			if((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if(rc != SQLITE_ROW){
				return SEKEY_ERR;
			}
			string key_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			string ksub = key_id.substr(1);
			uint32_t kid = stoul_wrap(ksub);
			keys_to_delete.push_back(kid);
		}
		query = "DELETE FROM Groups WHERE group_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, gid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
		}
		query = "DELETE FROM UserGroup WHERE group_id NOT IN (SELECT g.group_id FROM Groups g);";
		if(sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK){
			return SEKEY_ERR;
		}
		query = "DELETE FROM Users WHERE user_id <> ?1 AND user_id NOT IN (SELECT ug.user_id FROM UserGroup ug);";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, uid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
		}
		query = "DELETE FROM SeKeys WHERE key_owner NOT IN (SELECT group_id FROM Groups);";
		if(sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK){
			return SEKEY_ERR;
		}
		for(uint32_t k : keys_to_delete){
			SEcube->L1SEkey_DeleteKey(k); // do not return error if keys are not deleted because the garbage collector will take care of this
		}
	} else {
		query = "DELETE FROM UserGroup WHERE user_id = ?1 AND group_id = ?2;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, uid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, gid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
		}
		query =	"UPDATE Groups SET users_counter = users_counter - 1 WHERE group_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, gid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
		}
		query = "DELETE FROM Users WHERE user_id <> ?1 AND user_id NOT IN (SELECT ug.user_id FROM UserGroup ug);";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, currentuser.userid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
		}
	}
	return SEKEY_OK;
}
int usr_sql_exec(char *buffer, uint16_t bufsize){
	if(is_admin){ return SEKEY_ERR_AUTH; }
	if(buffer == nullptr){ return SEKEY_ERR; }
	string query(buffer, bufsize);
	stringstream ss(query);
	string sql;
	const string whitespace = " \n\r\t\f\v";
	while(getline(ss, sql, ';')){
		sql.append(";");
		size_t start = sql.find_first_not_of(whitespace);
		if(start == string::npos){
			return SEKEY_ERR;
		} else {
			sql = sql.substr(start);
		}
		if(sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK){
			return SEKEY_ERR;
		}
	}
	string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", executed SQL query from update file\n";
	logs.append(msg);
	return SEKEY_OK;
}
int usr_delete_user(char *buffer){
	if(is_admin){ return SEKEY_ERR_AUTH; }
	if(buffer == nullptr){ return SEKEY_ERR; }
	uint16_t uid_len, offset = 0;
	vector<uint32_t> keep;
	string query;
	statement sqlstmt;
	bool found = false;
	memcpy(&uid_len, buffer+offset, 2);
	offset+=2;
	string uid(buffer+offset, uid_len);
	bool this_user = false;
	int rc;
	if(uid.compare(currentuser.userid) == 0){
		this_user = true;
	}
	if(this_user){
		query = "SELECT k1, k2 FROM Users WHERE user_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, uid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
			return SEKEY_ERR;
		}
		for(;;){
			if((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if(rc != SQLITE_ROW){
				return SEKEY_ERR;
			}
			uint32_t k1 = get_u32(sqlstmt.getstmt(), 0);
			uint32_t k2 = get_u32(sqlstmt.getstmt(), 1);
			keep.push_back(uint32_t(L1Key::Id::RESERVED_ID_SEKEY_SECUREDB));
			keep.push_back(uint32_t(L1Key::Id::RESERVED_ID_SEKEY_WILDCARD));
			keep.push_back(uint32_t(L1Key::Id::RESERVED_ID_SETELEGRAM));
			keep.push_back(k1);
			keep.push_back(k2);
			found = true;
			/* the "found" flag is used to check if the database from the user side contains at leas the minimal info about
			 * the user himself. if it doesn't it means that basically the SEcube and the SEkey database are already empty,
			 * probably because the user has just been initialized. so avoid deleting the keys (because the flash of the
			 * SEcube already contains only the 4 keys that every SEcube has by default) and just delete the tables' content
			 * as precaution (but they should already be empty). notice that, if the user database contains this user, we
			 * keep the 4 keys in order to be able to execute again this update il case of failure. if the update goes well
			 * instead, we simply have 4 useless keys because the administrator is not going to send anything for us in future,
			 * moreover the garbage collector will delete k1 and k2 as soon as possible. */
		}
		if(found){
			if(!SEcube->L1SEkey_DeleteAllKeys(keep)){
				return SEKEY_ERR;
			}
		}
		vector<string> v;
		v.push_back(string("DELETE FROM Users;"));
		v.push_back(string("DELETE FROM Groups;"));
		v.push_back(string("DELETE FROM SeKeys;"));
		v.push_back(string("DELETE FROM UserGroup;"));
		v.push_back(string("DELETE FROM Recovery;"));
		for(string sql : v){
			if(sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK){
				return SEKEY_ERR;
			}
		}
	} else {
		query = "UPDATE Groups SET users_counter = users_counter - 1 WHERE group_id IN ("
				"SELECT ug.group_id FROM UserGroup ug WHERE ug.user_id = ?1);";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, uid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
		}
		query = "DELETE FROM UserGroup WHERE user_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, uid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
		}
		query = "DELETE FROM Users WHERE user_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, uid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
		}
	}
	string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", user " + uid + " deleted from sekey\n";
	logs.append(msg);
	return SEKEY_OK;
}
int usr_store_key(char *buffer){
	if(is_admin){ return SEKEY_ERR_AUTH; }
	if(buffer == nullptr){ return SEKEY_ERR; }
	uint16_t key_len, offset = 0;
	uint32_t key_id, dec_id = 0;
	statement sqlstmt;
	int rc;
	memcpy(&key_id, buffer+offset, 4);
	offset+=4;
	memcpy(&key_len, buffer+offset, 2);
	offset+=2;
	string query = "SELECT k2 FROM Users WHERE user_id = ?1;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, currentuser.userid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
		return SEKEY_ERR;
	}
	for(;;){
		if((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if(rc != SQLITE_ROW){
			return SEKEY_ERR;
		}
		dec_id = get_u32(sqlstmt.getstmt(), 0);
	}
	if(dec_id == 0){ // dec_id is the id of the key that the SEcube must use to decrypt the encrypted key that we are going to send
		return SEKEY_ERR;
	}
	shared_ptr<uint8_t[]> p1(new uint8_t[key_len]);
	memcpy(p1.get(), buffer+offset, key_len);
	if(!SEcube->L1SEkey_InsertKey(key_id, key_len, dec_id, p1)){
		return SEKEY_ERR;
	}
	string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", stored key " + to_string(key_id) + " in the secube flash memory\n";
	logs.append(msg);
	return SEKEY_OK;
}
int usr_delete_group(char *buffer){
	if(is_admin){ return SEKEY_ERR_AUTH; }
	if(buffer == nullptr){ return SEKEY_ERR; }
	string query;
	statement sqlstmt;
	vector<uint32_t> keys_delete;
	uint16_t gid_len, offset = 0;
	int rc;
	memcpy(&gid_len, buffer+offset, 2);
	offset+=2;
	string gid(buffer+offset, gid_len);
	/* physically delete the keys of the group from the SEcube internal memory */
	query = "SELECT key_id FROM SeKeys WHERE key_owner = ?1;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, gid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
		return SEKEY_ERR;
	}
	for(;;){
		if((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if(rc != SQLITE_ROW){
			return SEKEY_ERR;
		}
		string key_id = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
		string ksub = key_id.substr(1);
		uint32_t kid = stoul_wrap(ksub);
		keys_delete.push_back(kid);
	}
	query = "DELETE FROM Groups WHERE group_id = ?1;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, gid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
	   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
	}
	query = "DELETE FROM UserGroup WHERE group_id = ?1;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, gid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
	   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
	}
	query = "DELETE FROM Users WHERE user_id <> ?1 AND user_id NOT IN (SELECT ug.user_id FROM UserGroup ug);";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, currentuser.userid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
	   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
	}
	query = "DELETE FROM SeKeys WHERE key_owner = ?1;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, gid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
	   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
	}
	/* Physically delete the keys of this group from the device. */
	for(uint32_t k_id : keys_delete){
		SEcube->L1SEkey_DeleteKey(k_id); // don't return error if key deletion doesn't happen, garbage collector will take care of this in the worst case
	}
	string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", deleted group " + gid + " from sekey\n";
	logs.append(msg);
	return SEKEY_OK;
}

/* FUNCTIONS TO PERFORM SEKEY UPDATE */
int reset_user_recovery(string& user_id, string& sn){
	if(!is_admin){ return SEKEY_ERR_AUTH; }
	string query = "DELETE FROM Recovery WHERE user_id = ?1 AND serial_number = ?2;";
	statement sqlstmt;
	string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", user " + user_id + " with SN " + sn + " removed from recovery";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 2, sn.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
	   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
		return SEKEY_ERR;
	}
	sekey_printlog(msg);
	return SEKEY_OK;
}
int open_update_file(SEfile& updatefile, string& sn, bool overwrite, bool create, int mode){
	if(!is_admin){ return SEKEY_ERR; }
	try{
		string filepath = root + sn;
		switch(mode){
			case INIT:
				filepath.append(".init");
				break;
			case RECOVERY:
				filepath.append(".recovery");
				break;
			case NORMAL:
				filepath.append(".normal");
				break;
			default:
				return SEKEY_ERR;
		}
		int rc = file_exists(filepath);
		int32_t creation, mode_;
		if(rc == SEKEY_ERR){
			return rc;
		}
		/* if using a shared windows folder for the updates, delete an existing file before overriding (workaround to solve a bug) */
#ifdef SHARED_WINDOWS_FOLDER
		if(overwrite && rc == SEKEY_FILE_FOUND){
			if(!deletefile(nullptr, filepath)){
				return SEKEY_ERR;
			}
		}
#endif
		if(is_admin){
			mode_ = SEFILE_WRITE;
		} else {
			mode_ = SEFILE_READ;
		}
		if(overwrite || (rc==SEKEY_FILE_NOT_FOUND && create)){
			creation = SEFILE_NEWFILE;
		} else {
			creation = SEFILE_OPEN;
		}
		if(updatefile.secure_open((char*)filepath.c_str(), mode_, creation) != 0){
			return SEKEY_ERR;
		}
		return SEKEY_OK;
	} catch(...){
		return SEKEY_ERR;
	}
}
bool deletefile(SEfile *fileptr, std::string& filepath){
	string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", deleted file " + filepath + "\n";
	if(fileptr != nullptr){
		fileptr->secure_close();
	}
	char enc_filename[MAX_PATHNAME];
    memset(enc_filename, 0, MAX_PATHNAME*sizeof(char));
    if(crypto_filename((char*)filepath.c_str(), enc_filename, nullptr) != 0){ return false; }
    if(remove(enc_filename) != 0){ return false; }
    logs.append(msg);
	return true;
}
int process_update_file(){
	if(is_admin){ return SEKEY_ERR_AUTH; }
	int rc1, rc2;
	/* for the moment the path of the updates is hardcoded here. with the GUI it could be configured by the user. */
	string filepath = root + currentuser.device_sn;
	string initfile = filepath + ".init";
	string recoveryfile = filepath + ".recovery";
	string normalupdatefile = filepath + ".normal";
	rc1 = file_exists(initfile);
	if(rc1 == SEKEY_FILE_FOUND){ // process init file first
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_ERR; }
		if(execute_update(initfile) == SEKEY_OK){
			rc2 = commit_transaction(); // init file update completed
			if(rc2 == SEKEY_OK){
				sekey_printlog(logs);
				logs.clear();
				if(!deletefile(nullptr, initfile)){ // delete the update file if commit works
					return SEKEY_ERR;
				}
			} else {
				return rc2;
			}
		} else {
			return rollback_transaction(); // update failed
		}
	}
	if(rc1 == SEKEY_ERR){
		return SEKEY_ERR; // file_exists failed
	}
	rc1 = file_exists(recoveryfile);
	if(rc1 == SEKEY_FILE_FOUND){ // process recovery file as second
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_ERR; }
		if(execute_update(recoveryfile) == SEKEY_OK){
			rc2 = commit_transaction(); // recovery file update completed
			if(rc2 == SEKEY_OK){
				sekey_printlog(logs);
				logs.clear();
				if(!deletefile(nullptr, recoveryfile)){ // delete the update file if commit works
					return SEKEY_ERR;
				}
			} else {
				return rc2; // do not call rollback because already called by commit in case of failure
			}
		} else {
			return rollback_transaction(); // update failed
		}
	}
	if(rc1 == SEKEY_ERR){
		return SEKEY_ERR; // file_exists failed
	}
	rc1 = file_exists(normalupdatefile);
	if(rc1 == SEKEY_FILE_FOUND){ // process normal update as last
		if(sqlite3_exec(db, "BEGIN;", nullptr, nullptr, nullptr) != SQLITE_OK){ return SEKEY_ERR; }
		if(execute_update(normalupdatefile) == SEKEY_OK){
			rc2 = commit_transaction(); // recovery file update completed
			if(rc2 == SEKEY_OK){
				sekey_printlog(logs);
				logs.clear();
				deletefile(nullptr, normalupdatefile); // delete the update file if commit works
				/* if deletion of the normal update file does not work, this is not a problem because the
				 * normal update file is incremental and uses the counter to let the user recognize the records
				 * which have alread been processed. sooner or later the delete will work and in the meantime
				 * the user will skip the records that he already processed. */
			} else {
				return rc2;
			}
		} else {
			return rollback_transaction(); // update failed
		}
	}
	if(rc1 == SEKEY_ERR){
		return SEKEY_ERR; // file_exists failed
	}
	return SEKEY_OK;
}
int execute_update(string& filepath){
	if(is_admin){ return SEKEY_ERR_AUTH; }
	SEfile updatefile(SEcube);
	uint32_t filedim = 0, bytes_read, offset = 0;
	unique_ptr<char[]> filecontent;
	uint16_t bufsize = 0;
	uint8_t buftype;
	int64_t update_counter = 0, cnt = 0;
	statement sqlstmt;
	int pos, rc;
	/* retrieve current update_counter value from the Users table */
	string query = "SELECT update_counter FROM Users WHERE user_id = ?1 AND serial_number = ?2;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, currentuser.userid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 2, currentuser.device_sn.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
		return SEKEY_ERR;
	}
	for(;;){
		if((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if(rc != SQLITE_ROW) {
			return SEKEY_ERR;
		}
		update_counter = sqlite3_column_int64(sqlstmt.getstmt(), 0);
	}
	/* for the moment, since the update file is not big, we just allocate a buffer in RAM where we store the entire file content
	 * and then we process it. in future this could be changed in order to be more efficient with big files. */
	if(secure_getfilesize((char*)filepath.c_str(), &filedim, SEcube) != 0){
		return SEKEY_ERR;
	}
	filecontent = make_unique<char[]>(filedim);
	memset(filecontent.get(), 0, filedim);
	if((updatefile.secure_open((char*)filepath.c_str(), SEFILE_READ, SEFILE_OPEN) != 0) ||
	   (updatefile.secure_seek(0, &pos, SEFILE_BEGIN) != 0)){
		return SEKEY_ERR;
	}
	/* we copy the entire file content in RAM (as plaintext of course) */
	if((updatefile.secure_read((uint8_t*)filecontent.get(), filedim, &bytes_read) != 0) ||
	   (bytes_read != filedim)){
		return SEKEY_ERR;
	}
	while(offset < bytes_read){ /* we scan each TLV record of the update file applying the requested changes */
		unique_ptr<char[]> bufcontent;
		buftype = filecontent.get()[offset];
		offset++;
		memcpy(&cnt, filecontent.get()+offset, 8);
		offset+=8;
		memcpy(&bufsize, filecontent.get()+offset, 2);
		offset+=2;
		bufcontent = make_unique<char[]>(bufsize);
		memset(bufcontent.get(), 0, bufsize);
		memcpy(bufcontent.get(), filecontent.get()+offset, bufsize);
		offset+=bufsize;
		if((cnt!=0) && (cnt!=(update_counter+1))){
			if(cnt <= update_counter){
				continue; // already done
			} else {
				return SEKEY_ERR; // misaligned with admin (when cnt > (update_counter + 1))
			}
		}
		// go on only if cnt == user_counter + 1 or if cnt == 0
		if(cnt != 0){ // increment only when it is a normal update file
			update_counter++;
		} else {
			update_counter = 0; // reset when it is an init or recovery update
		}
		/* we use different types of commands: sometime it is easier to send directly the precomputed SQL queries from the admin to the users,
		 * sometimes is easier to send just a request to perform some operation according to some parameters. */
		switch(buftype){
		case SQL_QUERY:
			if(usr_sql_exec(bufcontent.get(), bufsize) != SEKEY_OK){
				return SEKEY_ERR;
			}
			break;
		case DELETE_USER_FROM_GROUP:
			if(usr_delete_user_from_group(bufcontent.get()) != SEKEY_OK){
				return SEKEY_ERR;
			}
			break;
		case DELETE_USER:
			if(usr_delete_user(bufcontent.get()) != SEKEY_OK){
				return SEKEY_ERR;
			}
			break;
		case DELETE_GROUP:
			if(usr_delete_group(bufcontent.get()) != SEKEY_OK){
				return SEKEY_ERR;
			}
			break;
		case KEY_DATA:
			if(usr_store_key(bufcontent.get()) != SEKEY_OK){
				return SEKEY_ERR;
			}
			break;
		default:
			return SEKEY_ERR;
		}
	}
	/* update counter for the user, this is a no-op in case the database was cleared because the user must be deleted. */
	query = "UPDATE Users SET update_counter = ?1 WHERE user_id = ?2 AND serial_number = ?3;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_int64(sqlstmt.getstmt(), 1, update_counter) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 2, currentuser.userid.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 3, currentuser.device_sn.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
	   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
		return SEKEY_ERR;
	}
	string msg = to_string(sekey_gettime()) + ", " + currentuser.userid + ", " + currentuser.device_sn + ", update counter of current user set to " + to_string(update_counter) + "\n";
	logs.append(msg);
	return SEKEY_OK;
}

/* FUNCTIONS TO WRITE DATA TO THE UPDATE FILE */
void req_delete_user_from_group(string& user_id, string& uid, string& group_id, bool erase){
	if(!is_admin){ return; }
	try{
		SEfile updatefile;
		int rc, pos;
		bool found = false;
		uint16_t offset = 0;
		// payload: 2B for length of user id to remove, 2B for length of group id (+ the space used by the strings)
		uint16_t payloadsize = 2 + uid.length() + 2 + group_id.length();
		uint32_t recordsize = payloadsize + UPDATE_RECORD_HEADER_LEN;
		uint32_t algo = 0, key_id = 0;
		int64_t cnt = 0;
		unique_ptr<uint8_t[]> buf = make_unique<uint8_t[]>(recordsize);
		string query, sn;
		statement sqlstmt;
		// retrieve the master-slave key to be used for this specific user
		query = "SELECT serial_number, k1, key_algo, update_counter FROM Users WHERE user_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
			return;
		}
		for(;;){
			if((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if(rc != SQLITE_ROW) {
				return;
			}
			found = true;
			sn = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			key_id = get_u32(sqlstmt.getstmt(), 1);
			algo = get_u32(sqlstmt.getstmt(), 2);
			cnt = sqlite3_column_int64(sqlstmt.getstmt(), 3);
			cnt++;
		}
		if(found == false){
			return; // don't add the user to the recovery list because the user apparently is not in the database
		}
		/* increment the update counter for the current user */
		query = "UPDATE Users SET update_counter = update_counter + 1 WHERE user_id = ?1;";
		if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK ||
		   sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
		   sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE){
			return;
		}
		buf[offset] = DELETE_USER_FROM_GROUP;
		offset++;
		memcpy(buf.get()+offset, &cnt, 8);
		offset+=8;
		memcpy(buf.get()+offset, &payloadsize, 2);
		offset+=2;
		uint16_t uid_len = uid.length();
		memcpy(buf.get()+offset, &uid_len, 2);
		offset+=2;
		memcpy(buf.get()+offset, uid.c_str(), uid_len);
		offset+=uid_len;
		uint16_t groupid_len = group_id.length();
		memcpy(buf.get()+offset, &groupid_len, 2);
		offset+=2;
		memcpy(buf.get()+offset, group_id.c_str(), groupid_len);
		if((updatefile.secure_init(SEcube, key_id, (uint16_t)algo) == 0) &&
		   (open_update_file(updatefile, sn, false, true, NORMAL) == SEKEY_OK) &&
		   (updatefile.secure_seek(0, &pos, SEFILE_END) == 0) &&
		   (updatefile.secure_write(buf.get(), recordsize) == 0) &&
		   (updatefile.secure_close() == 0) &&
		   (erase)){
				reset_user_recovery(user_id, sn);
		}
	} catch(...){
		/* Don't care about exceptions, this MUST be no throw to grant correct execution of higher level APIs.
		 * Notice that higher level APIs are written such that we already assume that this function will fail,
		 * in fact we don't do anything in case of error because the user has already been added to the recovery
		 * list. What we do is simply to remove the user from the recovery list in case everything goes well.
		 * It is easier to handle a single case when things go well instead of many cases where things go wrong. */
		return;
	}
}
void req_delete_user(string& user_id, string& uid, bool erase){
	if(!is_admin){ return; }
	SEfile updatefile;
	int rc, pos;
	bool found = false;
	uint16_t offset = 0;
	// 2B for length of user id to remove (+ the space used by the string)
	uint16_t payloadsize = 2 + uid.length();
	uint32_t recordsize = payloadsize + UPDATE_RECORD_HEADER_LEN;
	unique_ptr<uint8_t[]> buf = make_unique<uint8_t[]>(recordsize);
	string query, sn;
	statement sqlstmt;
	uint32_t algo = 0, key_id = 0;
	int64_t cnt = 0;
	// retrieve the master-slave key to be used for this specific user
	query = "SELECT serial_number, k1, key_algo, update_counter FROM Users WHERE user_id = ?1;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
		return;
	}
	for(;;){
		if((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if(rc != SQLITE_ROW) {
			return;
		}
		found = true;
		sn = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
		key_id = get_u32(sqlstmt.getstmt(), 1);
		algo = get_u32(sqlstmt.getstmt(), 2);
		cnt = sqlite3_column_int64(sqlstmt.getstmt(), 3);
		cnt++;
	}
	if(found == false){
		return; // don't add the user to the recovery list because the user apparently is not in the database
	}
	/* increment the update counter for the current user */
	query = "UPDATE Users SET update_counter = update_counter + 1 WHERE user_id = ?1;";
	if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK ||
	   sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
	   sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE){
		return;
	}
	buf[offset] = DELETE_USER;
	offset++;
	memcpy(buf.get()+offset, &cnt, 8);
	offset+=8;
	memcpy(buf.get()+offset, &payloadsize, 2);
	offset+=2;
	uint16_t uid_len = uid.length();
	memcpy(buf.get()+offset, &uid_len, 2);
	offset+=2;
	memcpy(buf.get()+offset, uid.c_str(), uid_len);
	if((updatefile.secure_init(SEcube, key_id, (uint16_t)algo) == 0) &&
	   (open_update_file(updatefile, sn, false, true, NORMAL) == SEKEY_OK) &&
	   (updatefile.secure_seek(0, &pos, SEFILE_END) == 0) &&
	   (updatefile.secure_write(buf.get(), recordsize) == 0) &&
	   (updatefile.secure_close() == 0) &&
	   (erase)){
			reset_user_recovery(user_id, sn);
	}
}
void req_delete_user(string& user_id, uint32_t algo, uint32_t key_id, string& sn, bool erase, int mode){
	if(!is_admin){ return; }
	try{
		SEfile updatefile;
		int pos;
		uint16_t offset = 0;
		// 2B for length of user id to remove (+ the space used by the string)
		uint16_t payloadsize = 2 + user_id.length();
		uint32_t recordsize = payloadsize + UPDATE_RECORD_HEADER_LEN;
		unique_ptr<uint8_t[]> buf = make_unique<uint8_t[]>(recordsize);
		buf[offset] = DELETE_USER;
		offset++;
		memset(buf.get()+offset, 0, 8);
		offset+=8;
		memcpy(buf.get()+offset, &payloadsize, 2);
		offset+=2;
		uint16_t uid_len = user_id.length();
		memcpy(buf.get()+offset, &uid_len, 2);
		offset+=2;
		memcpy(buf.get()+offset, user_id.c_str(), uid_len);
		if((updatefile.secure_init(SEcube, key_id, (uint16_t)algo) == 0) &&
		   /* override the update file because the user must delete the entire content of SEkey so we don't
		    * care about what's inside the update file before this very record */
		   (open_update_file(updatefile, sn, true, true, mode) == SEKEY_OK) &&
		   (updatefile.secure_seek(0, &pos, SEFILE_END) == 0) &&
		   (updatefile.secure_write(buf.get(), recordsize) == 0) &&
		   (updatefile.secure_close() == 0) &&
		   (erase)){
				reset_user_recovery(user_id, sn);
		}
	} catch(...){
		/* Don't care about exceptions, this MUST be no throw to grant correct execution of higher level APIs.
		 * Notice that higher level APIs are written such that we already assume that this function will fail,
		 * in fact we don't do anything in case of error because the user has already been added to the recovery
		 * list. What we do is simply to remove the user from the recovery list in case everything goes well.
		 * It is easier to handle a single case when things go well instead of many cases where things go wrong. */
		return;
	}
}
void req_delete_group(string& user_id, string& gid, bool erase){
	if(!is_admin){ return; }
	SEfile updatefile;
	int rc, pos;
	bool found = false;
	uint16_t offset = 0;
	// 2B for length of user id to remove (+ the space used by the string)
	uint16_t payloadsize = 2 + gid.length();
	uint32_t recordsize = payloadsize + UPDATE_RECORD_HEADER_LEN;
	unique_ptr<uint8_t[]> buf = make_unique<uint8_t[]>(recordsize);
	string query, sn;
	statement sqlstmt;
	uint32_t algo = 0, key_id = 0;
	int64_t cnt = 0;
	// retrieve the master-slave key to be used for this specific user
	query = "SELECT serial_number, k1, key_algo, update_counter FROM Users WHERE user_id = ?1;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
		return;
	}
	for(;;){
		if((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if(rc != SQLITE_ROW) {
			return;
		}
		found = true;
		sn = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
		key_id = get_u32(sqlstmt.getstmt(), 1);
		algo = get_u32(sqlstmt.getstmt(), 2);
		cnt = sqlite3_column_int64(sqlstmt.getstmt(), 3);
		cnt++;
	}
	if(found == false){
		return; // don't add the user to the recovery list because the user apparently is not in the database
	}
	query = "UPDATE Users SET update_counter = update_counter + 1 WHERE user_id = ?1;";
	if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK ||
	   sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
	   sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE){
		return;
	}
	buf[offset] = DELETE_GROUP;
	offset++;
	memcpy(buf.get()+offset, &cnt, 8);
	offset+=8;
	memcpy(buf.get()+offset, &payloadsize, 2);
	offset+=2;
	uint16_t uid_len = gid.length();
	memcpy(buf.get()+offset, &uid_len, 2);
	offset+=2;
	memcpy(buf.get()+offset, gid.c_str(), uid_len);
	if((updatefile.secure_init(SEcube, key_id, (uint16_t)algo) == 0) &&
	   (open_update_file(updatefile, sn, false, true, NORMAL) == SEKEY_OK) &&
	   (updatefile.secure_seek(0, &pos, SEFILE_END) == 0) &&
	   (updatefile.secure_write(buf.get(), recordsize) == 0) &&
	   (updatefile.secure_close() == 0) &&
	   (erase)){
		reset_user_recovery(user_id, sn);
	}
}
void send_key_update(string& user_id, uint32_t kid, uint32_t key_len, bool erase){
	if(!is_admin){ return; }
	SEfile updatefile;
	int rc, pos;
	bool found = false;
	statement sqlstmt;
	uint32_t algo = 0, k1 = 0, k2 = 0;
	shared_ptr<uint8_t[]> key_data; // the buffer that will hold the encrypted key content
	string query, sn;
	int64_t cnt = 0;
	// retrieve the elements required to send the update to the user
	query = "SELECT serial_number, k1, k2, key_algo, update_counter FROM Users WHERE user_id = ?1;";
	if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK ||
	   sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
		return;
	}
	for(;;){
		if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			return;
		}
		found = true;
		sn = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
		k1 = get_u32(sqlstmt.getstmt(), 1);
		k2 = get_u32(sqlstmt.getstmt(), 2);
		algo = get_u32(sqlstmt.getstmt(), 3);
		cnt = sqlite3_column_int64(sqlstmt.getstmt(), 4);
		cnt++;
	}
	if(found == false){
		return;
	}
	// retrieve the encrypted key content
	uint16_t key_data_len = 0;
	if(!SEcube->L1SEkey_GetKeyEnc(kid, k2, key_data, key_data_len)){
		return;
	}
	if(key_data == nullptr){
		return;
	}
	/* increment the update counter for the current user */
	query = "UPDATE Users SET update_counter = update_counter + 1 WHERE user_id = ?1;";
	if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK ||
	   sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
	   sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE){
		return;
	}
	uint16_t payloadsize = key_data_len+2+4, offset = 0; /* bufsize made of key ID (4B), key length (2B) and key data */
	uint32_t recordsize = payloadsize + UPDATE_RECORD_HEADER_LEN;
	unique_ptr<uint8_t[]> buf = make_unique<uint8_t[]>(recordsize);
	buf[offset] = KEY_DATA; // command type
	offset++;
	memcpy(buf.get()+offset, &cnt, 8);
	offset+=8;
	memcpy(buf.get()+offset, &payloadsize, 2); // payload length
	offset+=2;
	memcpy(buf.get()+offset, &kid, 4); // key ID
	offset+=4;
	memcpy(buf.get()+offset, &key_data_len, 2); // key length
	offset+=2;
	memcpy(buf.get()+offset, key_data.get(), key_data_len); // key data
	if((updatefile.secure_init(SEcube, k1, (uint16_t)algo) == 0) &&
	   (open_update_file(updatefile, sn, false, true, NORMAL) == SEKEY_OK) &&
	   (updatefile.secure_seek(0, &pos, SEFILE_END) == 0) &&
	   (updatefile.secure_write(buf.get(), recordsize) == 0) &&
	   (updatefile.secure_close() == 0) &&
	   (erase)){
		reset_user_recovery(user_id, sn);
	}
}
void send_sql_update(string& user_id, string& query, bool erase){
	if(!is_admin){ return; }
	SEfile updatefile;
	int rc, pos;
	bool found = false;
	uint16_t payloadsize = query.length();
	uint32_t recordsize = payloadsize + UPDATE_RECORD_HEADER_LEN;
	statement sqlstmt;
	uint32_t algo = 0, key_id = 0;
	/* the buffer to be written in the update file is organized in this way:
	 * 1 byte for type field | 8 bytes for update_counter field | 2 bytes for length field | N bytes for the value field where N depends on the data that must be written (N = content of length field) */
	/* the type field is used to distinguish the type of operation to be done, the update_counter field is used by the user application to check if that specific operation has already be done (useful
	 * in case the same update data is processed multiple times even after those modifications have already been applied, this prevents the user from executing more than one time the same operation
	 * like an SQL query resulting in an error), the length field is the length of the value field, the value field can be an SQL query or a special request. */
	unique_ptr<uint8_t[]> buf = make_unique<uint8_t[]>(recordsize);
	string query2, sn;
	uint16_t offset = 0;
	int64_t cnt = 0;
	// retrieve the elements required to write the update file for the user
	query2 = "SELECT serial_number, k1, key_algo, update_counter FROM Users WHERE user_id = ?1;";
	if(sqlite3_prepare_v2(db, query2.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK ||
	   sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
		return;
	}
	for(;;){
		if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			return;
		}
		found = true;
		sn = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
		key_id = get_u32(sqlstmt.getstmt(), 1);
		algo = get_u32(sqlstmt.getstmt(), 2);
		cnt = sqlite3_column_int64(sqlstmt.getstmt(), 3);
		cnt++;
	}
	if(found == false){
		return;
	}
	/* increment the update counter for the current user */
	query2 = "UPDATE Users SET update_counter = update_counter + 1 WHERE user_id = ?1;";
	if(sqlite3_prepare_v2(db, query2.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK ||
	   sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
	   sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE){
		return;
	}
	/* Notice that the current user is removed from the list of users who need recovery only if both the update file and the update counter are correctly modified.
	 * Therefore if the update file is written correctly but, for any reason, the user counter is not updated, the user will still be in recovery and will be
	 * forced to erase its database when performing the recovery. The only drawback of this strategy is that, if the administrator does not write the recovery file
	 * for the user soon enough, the user in the meantime is not aware of the fact that he needs recovery so he will keep using a database which is not updated, or
	 * in case the update file was written, he may perform the update followed by an immediate recovery (i.e. because the update file was generated correctly but
	 * the counter increment failed so the administrator will force the user to recover the database even if the one who has "incorrect" data is the admin who has
	 * an old value for the counter). When the recovery is performed by the admin, the update counter is also reset to 0. */
	// fill the buffer
	buf[offset] = SQL_QUERY;
	offset++;
	memcpy(buf.get()+offset, &cnt, 8);
	offset+=8;
	memcpy(buf.get()+offset, &payloadsize, 2);
	offset+=2;
	memcpy(buf.get()+offset, query.c_str(), payloadsize);
	/* all the functions in this if statement must be performed in this order. they are written directly
	 * inside the if condition because the evaluation of the if clause goes from left to right therefore
	 * because of the OR the condition will be met by the first function that fails. */
	if((updatefile.secure_init(SEcube, key_id, (uint16_t)algo) == 0) &&
	   (open_update_file(updatefile, sn, false, true, NORMAL) == SEKEY_OK) &&
	   (updatefile.secure_seek(0, &pos, SEFILE_END) == 0) &&
	   (updatefile.secure_write(buf.get(), recordsize) == 0) &&
	   (updatefile.secure_close() == 0) &&
	   (erase)){
		reset_user_recovery(user_id, sn);
	}
}
int send_user_init_update(string& user_id, string& query){
	if(!is_admin){ return SEKEY_ERR; }
	SEfile updatefile;
	int rc, pos;
	bool found = false;
	uint16_t payloadsize = query.length();
	uint32_t recordsize = payloadsize + UPDATE_RECORD_HEADER_LEN;
	unique_ptr<uint8_t[]> buf = make_unique<uint8_t[]>(recordsize);
	string query2, sn;
	statement sqlstmt;
	uint32_t algo, key_id;
	uint16_t offset = 0;
	// retrieve the master-slave key to be used for this specific user
	query2 = "SELECT serial_number, k1, key_algo FROM Users WHERE user_id = ?1;";
	if(sqlite3_prepare_v2(db, query2.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK ||
	   sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC) != SQLITE_OK){
		return SEKEY_ERR;
	}
	for(;;){
		if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			return SEKEY_ERR;
		}
		found = true;
		sn = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
		key_id = get_u32(sqlstmt.getstmt(), 1);
		algo = get_u32(sqlstmt.getstmt(), 2);
	}
	if(found == false){
		return SEKEY_ERR;
	}
	// fill the buffer
	buf[offset] = SQL_QUERY; // T
	offset++;
	memset(buf.get()+offset, 0, 8); // C
	offset+=8;
	memcpy(buf.get()+offset, &payloadsize, 2); // L
	offset+=2;
	memcpy(buf.get()+offset, query.c_str(), payloadsize); // V
	/* all the functions in this if statement must be performed in this order. they are written directly
	 * inside the if condition because the evaluation of the if clause goes from left to right therefore
	 * because of the OR the condition will be met by the first function that fails. */
	if((updatefile.secure_init(SEcube, key_id, (uint16_t)algo) != 0) ||
	   (open_update_file(updatefile, sn, true, true, INIT) != SEKEY_OK) || // here overwrite flag MUST be true (in case we have to rewrite the update after it failed)
	   (updatefile.secure_seek(0, &pos, SEFILE_END) != 0) ||
	   (updatefile.secure_write(buf.get(), recordsize) != 0) ||
	   (updatefile.secure_close() != 0)){
			return SEKEY_ERR;
	}
	return SEKEY_OK;
}

/* DATABASE AND SECUBE MAINTENANCE OPERATIONS */
int rollback_transaction(){
	try{
		for(int i=0; i<3; i++){
			if(sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr) == SQLITE_OK){
				return SEKEY_UNCHANGED;
			}
#ifdef _WIN32
			Sleep(300); // sleeping for some time may be useful because maybe the issue is transient
#elif defined(__linux__) || defined(__APPLE__)
			usleep(300*1000);
#endif
		}
		return SEKEY_RESTART;
	} catch(...){
		 /* in case of any exception is reasonable to ask to the caller to restart the
		  * application in order to go back to a coherent database state. */
		return SEKEY_RESTART;
	}
}
int commit_transaction(){
	try{
		for(int i=0; i<3; i++){
			int rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
			if(rc == SQLITE_OK){
				return SEKEY_OK;
			}
#ifdef _WIN32
			Sleep(300); // in case of a transient error, sleeping for some time may be useful
#elif defined(__linux__) || defined(__APPLE__)
			usleep(300*1000);
#endif
		}
	} catch(...){
		return SEKEY_RESTART;
	}
	/* rollback in case the commit is not possible */
	return rollback_transaction();
}

uint32_t keyIDclass(uint32_t id){
	if((id >= L1Key::Id::MANUAL_ID_BEGIN) && (id <= L1Key::Id::MANUAL_ID_END)){
		return L1Key::IdClass::MANUAL;
	}
	if((id >= L1Key::Id::RESERVED_ID_SECUBE_BEGIN) && (id <= L1Key::Id::RESERVED_ID_SECUBE_END)){
		return L1Key::IdClass::RESERVED_SECUBE;
	}
	if((id >= L1Key::Id::RESERVED_ID_SEKEY_BEGIN) && (id <= L1Key::Id::RESERVED_ID_SEKEY_END)){
		return L1Key::IdClass::RESERVED_SEKEY;
	}
	if((id >= L1Key::Id::SEKEY_ID_BEGIN) && (id <= L1Key::Id::SEKEY_ID_END)){
		return L1Key::IdClass::KMS;
	}
	return L1Key::IdClass::INVALID;
}

void se3_flash_maintenance_routine(){
	try{
		/* retrieve all the IDs of the keys inside the flash and send a command to delete them if they are not part of SEkey */
		uint16_t resp_len, offset, keylen;
		unique_ptr<uint8_t[]> buffer = make_unique<uint8_t[]>(L1Response::Size::MAX_DATA); // allocate few more bytes than the limit inside the firmware API (the L1SEkeyMaintenance command will receive max 6004 B in theory)
		uint32_t key_id;
		string query;
		statement sqlstmt;
		int rc;
		bool remove;
		for(;;){ // exit from the for(;;) loop when the L1SEkeyMaintenance will return resp_len = 0 (entire flash scanned for unrequired keys)
			offset = 0;
			resp_len = 0;
			memset(buffer.get(), 0, L1Response::Size::MAX_DATA);
			SEcube->L1SEkey_Maintenance(buffer.get(), &resp_len);
			if((resp_len == 0) || ((resp_len % 6) != 0)){ // % 6 because 4B for ID and 2B for key length
				return;
			}
			while(offset < resp_len){
				key_id = 0; // reset id
				keylen = 0;
				remove = false;
				memcpy(&key_id, buffer.get()+offset, 4);
				offset+=4;
				memcpy(&keylen, buffer.get()+offset, 2);
				offset+=2;
				if(key_id == 0){ // the SEcube reached the end of the flash
					return;
				}
				if(keyIDclass(key_id) == L1Key::IdClass::MANUAL || keyIDclass(key_id) == L1Key::IdClass::RESERVED_SECUBE){
					continue; // nothing to do here
				}
				if(keyIDclass(key_id) == L1Key::IdClass::RESERVED_SEKEY){
					query = "SELECT COUNT(*) FROM Users WHERE k1 = ?1 OR k2 = ?2;";
					if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)!=SQLITE_OK) ||
					   (sqlite3_bind_int64(sqlstmt.getstmt(), 1, key_id) != SQLITE_OK) ||
					   (sqlite3_bind_int64(sqlstmt.getstmt(), 2, key_id) != SQLITE_OK)){
							continue;
					}
					for(;;){
						if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
							break;
						}
						if (rc != SQLITE_ROW) {
							break;
						}
						if(sqlite3_column_int64(sqlstmt.getstmt(), 0) == 0){
							remove = true;
						}
					}
				}
				if(keyIDclass(key_id) == L1Key::IdClass::KMS){
					query = "SELECT COUNT(*) FROM SeKeys WHERE key_id = ?1;";
					string _tmp = "K" + to_string(key_id);
					if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
					   (sqlite3_bind_text(sqlstmt.getstmt(), 1, _tmp.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
							continue;
					}
					for(;;){
						if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
							break;
						}
						if (rc != SQLITE_ROW) {
							break;
						}
						if(sqlite3_column_int64(sqlstmt.getstmt(), 0) == 0){
							remove = true;
						}
					}
					if(!remove){ // if the key is in the database, check if its status is "destroyed". in this case the key must be deleted from the flash of the device.
						query = "SELECT status FROM SeKeys WHERE key_id = ?1;";
						if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
						   (sqlite3_bind_text(sqlstmt.getstmt(), 1, _tmp.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
								continue;
						}
						for(;;){
							if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
								break;
							}
							if (rc != SQLITE_ROW) {
								break;
							}
							if(sqlite3_column_int64(sqlstmt.getstmt(), 0) == (int64_t)se_key_status::destroyed){
								remove = true;
							}
						}
					}
				}
				if(keyIDclass(key_id) == L1Key::IdClass::INVALID){
					remove = true;
				}
				if(remove){
					remove = false;
					SEcube->L1SEkey_DeleteKey(key_id); // don't check if deletion was successful, worst case is we issue the deletion again the next time...
				}
			}
		}
	} catch (...) {
		/* do nothing...if garbage collector doesn't work now is not a problem. SEkey is still usable, the
		 * worst case is that the SEcube has keys in the flash that it should not have but the garbage collector
		 * will take care of this as soon as possible (next API call) */
	}
	/* Warning: at the moment the routine to clean the SEcube flash from unused keys works only with keys which are stored in the
	 * flash but are not stored in the SEcube database. It could be useful to add another functionality: delete from the database
	 * the keys which can't be found in the SEcube flash. This could avoid potential problems when using the API to find the key
	 * to be used for encryption, in that case the API could return an ID which is not related to a key which is actually stored
	 * in the flash, therefore the APIs for encryption and decryption would fail later. In order to implement this functionality
	 * should be enough to read all the keys from the database (both Keys table and Users table) and call, for each key, the corresponding
	 * L1FindKey API. If the API returns key not found, then the key must be deleted from the table. */
}

/* OTHER USEFUL FUNCTIONS */
bool check_input(string& in, uint8_t sel){
	regex regex_userid("^[U]{1}[1-9]+[0-9]*$"); /**< Regex used to check the user ID provided to the SEkey API. Defined as global variable to avoid defining it in each API. */
	regex regex_groupid("^[G]{1}[1-9]+[0-9]*$"); /**< Regex used to check the group ID provided to the SEkey API. Defined as global variable to avoid defining it in each API. */
	regex regex_keyid("^[K]{1}[1-9]+[0-9]*$"); /**< Regex used to check the key ID provided to the SEkey API. Defined as global variable to avoid defining it in each API. */
	if(sel == 0){
		return regex_match(in, regex_userid);
	}
	if(sel == 1){
		return regex_match(in, regex_groupid);
	}
	if(sel == 2){
		return regex_match(in, regex_keyid);
	}
	return false;
}
bool user_allowed(){
	if(!is_admin){
		blocked = true;
		if(sekey_update_userdata() == SEKEY_OK){
			blocked = false;
			return true;
		} else {
			blocked = true;
			return false;
		}
	}
	return true; // return true if admin mode or if user is up to date with latest SEkey data
}
int algocmp(uint32_t algo1, uint32_t algo2){
	/* this function must be implemented depending on the algorithms available */
	if(algo1 == algo2){
		return 0;
	}
	// algo are different
	if(algo1 == L1Algorithms::Algorithms::AES_HMACSHA256){
		return 1; // 1st choice
	}
	if(algo2 == L1Algorithms::Algorithms::AES_HMACSHA256){
		return -1;
	}
	if(algo1 == L1Algorithms::Algorithms::AES){
		return 1; // 2nd choice
	}
	if(algo2 == L1Algorithms::Algorithms::AES){
		return -1;
	}
	return 0; // if not among these return 0 and use other criteria
}
uint32_t get_u32(sqlite3_stmt *stmt, int index){
	int64_t rc;
	if(stmt == nullptr){
		throw std::invalid_argument("stmt is null");
	}
	rc = sqlite3_column_int64(stmt, index);
	if(rc > UINT32_MAX || rc < 0){
		throw std::out_of_range("The value can't be stored as uint32_t.");
	}
	return rc;
}
int sqlite3_expanded_sql_wrapper(sqlite3_stmt *stmt, string& s){
	if(stmt == nullptr){
		return SEKEY_ERR;
	}
	char *c = nullptr;
	c = sqlite3_expanded_sql(stmt);
	if(c == nullptr){
		return SEKEY_ERR;
	}
	try{
		s = string(c);
	} catch (...) { // try catch to avoid memory leakage on pointer c
		sqlite3_free(c);
		return SEKEY_ERR;
	}
	sqlite3_free(c);
	return SEKEY_OK;
}
string sqlite3_column_text_wrapper(sqlite3_stmt *stmt, int col){
	try{
		string s = "";
		if(stmt == nullptr){
			return s;
		}
		char *c = nullptr;
		int n = 0;
		c = (char*)sqlite3_column_text(stmt, col);
		if(c == nullptr){
			return s;
		}
		n = sqlite3_column_bytes(stmt, col);
		s = string(c, n);
		return s;
	} catch (...) {
		string s = "";
		return s;
	}
}
int generate_serial_number(array<uint8_t, L0Communication::Size::SERIAL>& sn){
	int r, c;
	char tmp[L0Communication::Size::SERIAL];
	memset(tmp, '\0', L0Communication::Size::SERIAL);
	uint8_t rnd[L0Communication::Size::SERIAL];
	memset(rnd, 0, L0Communication::Size::SERIAL);
	L0Support::Se3Rand(L0Communication::Size::SERIAL, rnd);
	srand(sekey_gettime());
	for(int j=0; j<L0Communication::Size::SERIAL; j++){
		c = rand()%3;
		switch(c){
			case 0: // 0-9 number
				r = rnd[j]%10;
				sn[j] = '0' + r;
				break;
			case 1: // a-z letter
				r = rnd[j]%26;
				sn[j] = 'a' + r;
				break;
			case 2: // A-Z letter
				r = rnd[j]%26;
				sn[j] = 'A' + r;
				break;
			default:
				r = rnd[j]%10;
				sn[j] = '0' + r;
		}
	}
	int ret = 0;
	time_t currtime = sekey_gettime();
	if(currtime == -1){
		return -1;
	}
#ifdef _WIN32
	if(is_signed<time_t>()){
		ret = sprintf(tmp, "%lld", (long long int)currtime);
	} else {
		ret = sprintf(tmp, "%llu", (unsigned long long int)currtime);
	}
#elif defined(__linux__) || defined(__APPLE__)
	if(is_signed<time_t>()){
		ret = sprintf(tmp, "%lld", (long long int)currtime);
	} else {
		ret = sprintf(tmp, "%llu", (unsigned long long int)currtime);
	}
#endif
	if(ret <= 0){
		return -1;
	}
	if(ret > L0Communication::Size::SERIAL){
		return -1; // time is longer than the serial number
	}
	for(int i=0; i<ret; i++){
		sn[i] = tmp[i]; // copy time
	}
	return 0;
}
int fill_recovery(vector<string>& users){
	if(!is_admin){ return SEKEY_ERR; }
	string query, sn;
	statement sqlstmt;
	bool found = false;
	int rc;
	for(string& curr_user : users){
		query = "SELECT serial_number FROM Users WHERE user_id = ?1;";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, curr_user.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
			return SEKEY_ERR;
		}
		for(;;){
			if((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
				break;
			}
			if(rc != SQLITE_ROW){
				return SEKEY_ERR;
			}
			sn = sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0);
			found = true;
		}
		if(!found){
			return SEKEY_ERR;
		} else {
			found = false; // reset for next iteration
		}
		query = "INSERT OR IGNORE INTO Recovery(user_id, serial_number) VALUES(?1, ?2);";
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 1, curr_user.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_bind_text(sqlstmt.getstmt(), 2, sn.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) ||
		   (sqlite3_step(sqlstmt.getstmt()) != SQLITE_DONE)){
			return SEKEY_ERR;
		}
	}
	return SEKEY_OK;
}
int file_exists(string& filename){
	try{
		char enc_filename[MAX_PATHNAME];
		memset(enc_filename, '\0', MAX_PATHNAME);
		if(crypto_filename((char*)filename.c_str(), enc_filename, nullptr)){
			return SEKEY_ERR;
		}
		ifstream myfile(enc_filename);
		if(myfile.good()){
			return SEKEY_FILE_FOUND;
		} else {
			return SEKEY_FILE_NOT_FOUND;
		}
	} catch(...){
		return SEKEY_ERR;
	}
}
int sql_fill_vector(string *bind, string& query, vector<string> *container){
	statement sqlstmt;
	string str;
	int rc;
	if(container == nullptr){
		return SEKEY_ERR_PARAMS;
	}
	if(bind != nullptr){
		if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK) ||
			   (sqlite3_bind_text(sqlstmt.getstmt(), 1, bind->c_str(), -1, SQLITE_STATIC) != SQLITE_OK)){
				return SEKEY_ERR;
			}
	} else {
		if(sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr) != SQLITE_OK){
			return SEKEY_ERR;
		}
	}
	for(;;){
		if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			return SEKEY_ERR;
		}
		str.assign(sqlite3_column_text_wrapper(sqlstmt.getstmt(), 0));
		container->push_back(str);
	}
	return SEKEY_OK;
}
int check_key_transition_validity(se_key_status current_status, se_key_status new_status){
	if(current_status == new_status){ // cannot re-assign the same status
		return SEKEY_ERR;
	}
	switch(current_status){
		case se_key_status::preactive: // preactive can change to any status except to suspended
			if(new_status == se_key_status::suspended){
				return SEKEY_ERR;
			}
			break;
		case se_key_status::active: // active can change to any status except to preactive
			if(new_status == se_key_status::preactive){
				return SEKEY_ERR;
			}
			break;
		case se_key_status::suspended: // suspended can change to any status except to preactive
			if(new_status == se_key_status::preactive){
				return SEKEY_ERR;
			}
			break;
		case se_key_status::deactivated: // deactivated can only change to compromised or destroyed
			if(new_status != se_key_status::destroyed && new_status != se_key_status::compromised){
				return SEKEY_ERR;
			}
			break;
		case se_key_status::compromised: // compromised can only change to destroyed
			if(new_status != se_key_status::destroyed){
				return SEKEY_ERR;
			}
			break;
		case se_key_status::destroyed: // destroyed status cannot be changed
			return SEKEY_ERR;
		default:
			return SEKEY_ERR;
	}
	return SEKEY_OK;
}
int is_user_present(string& user_id){
	statement sqlstmt;
	int rc;
	bool found = false;
	if(user_id.empty() || user_id.length()>IDLEN){
		return SEKEY_ERR_PARAMS;
	}
	string query = "SELECT COUNT(*) FROM Users WHERE user_id = ?1;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)!=SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, user_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
		return SEKEY_ERR;
	}
	for(;;){
		if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			return SEKEY_ERR;
		}
		if(sqlite3_column_int64(sqlstmt.getstmt(), 0) >= 1){
			found = true;
		}
	}
	if(found == false){
		return SEKEY_USER_NOT_FOUND;
	}
	return SEKEY_OK;
}
int is_group_present(string& group_id){
	statement sqlstmt;
	int rc;
	bool found = false;
	if(group_id.empty() || group_id.length()>IDLEN){
		return SEKEY_ERR_PARAMS;
	}
	string query = "SELECT COUNT(*) FROM Groups WHERE group_id = ?1;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)!=SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, group_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
		return SEKEY_ERR;
	}
	for(;;){
		if ((rc = sqlite3_step(sqlstmt.getstmt())) == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			return SEKEY_ERR;
		}
		if(sqlite3_column_int64(sqlstmt.getstmt(), 0) >= 1){
			found = true;
		}
	}
	if(found == false){
		return SEKEY_GROUP_NOT_FOUND;
	}
	return SEKEY_OK;
}
int is_key_present(string& key_id){
	statement sqlstmt;
	int rc;
	bool found = false;
	if(key_id.empty() || key_id.length()>IDLEN){
		return SEKEY_ERR_PARAMS;
	}
	string query = "SELECT COUNT(*) FROM SeKeys WHERE key_id = ?1;";
	if((sqlite3_prepare_v2(db, query.c_str(), -1, sqlstmt.getstmtref(), nullptr)!=SQLITE_OK) ||
	   (sqlite3_bind_text(sqlstmt.getstmt(), 1, key_id.c_str(), -1, SQLITE_STATIC)!=SQLITE_OK)){
		return SEKEY_ERR;
	}
	for(;;){
		rc = sqlite3_step(sqlstmt.getstmt());
		if (rc == SQLITE_DONE){
			break;
		}
		if (rc != SQLITE_ROW) {
			return SEKEY_ERR;
		}
		if(sqlite3_column_int64(sqlstmt.getstmt(), 0) >= 1){
			found = true;
		}
	}
	if(found == false){
		return SEKEY_KEY_NOT_FOUND;
	}
	return SEKEY_OK;
}
uint32_t algolen(uint32_t algorithm){
	uint32_t rc;
	switch (algorithm){
	case L1Algorithms::Algorithms::AES_HMACSHA256:
		rc = 32;
		break;
	default:
		rc = 0;
	}
	// value returned in byte, not bit
	return rc;
}
bool algovalid(uint32_t algorithm){
	switch (algorithm){
	case L1Algorithms::Algorithms::AES_HMACSHA256:
		return true;
	case L1Algorithms::Algorithms::AES:
		return true;
	case L1Algorithms::Algorithms::HMACSHA256:
		return true;
	default:
		return false;
	}
}
string statusmap(se_key_status s){
	string str;
	switch(s){
	case se_key_status::preactive:
		str = "preactive";
		break;
	case se_key_status::active:
		str = "active";
		break;
	case se_key_status::deactivated:
		str = "deactivated";
		break;
	case se_key_status::suspended:
		str = "suspended";
		break;
	case se_key_status::compromised:
		str = "compromised";
		break;
	case se_key_status::destroyed:
		str = "destroyed";
		break;
	default:
		str = "invalid status";
	}
	return str;
}
string keytypemap(se_key_type t){
	string str;
	switch(t){
	case se_key_type::private_signature:
		str = "private signature";
		break;
	case se_key_type::public_signature_verification:
		str = "public signature verification";
		break;
	case se_key_type::symmetric_authentication:
		str = "symmetric authentication";
		break;
	case se_key_type::private_authentication:
		str = "private authentication";
		break;
	case se_key_type::public_authentication:
		str = "public authentication";
		break;
	case se_key_type::symmetric_data_encryption:
		str = "symmetric data encryption";
		break;
	case se_key_type::symmetric_key_wrapping:
		str = "symmetric key wrapping";
		break;
	case se_key_type::symmetric_RGB:
		str = "symmetric RGB";
		break;
	case se_key_type::symmetric_key_derivation:
		str = "symmetric key derivation";
		break;
	case se_key_type::private_key_transport:
		str = "private key transport";
		break;
	case se_key_type::public_key_transport:
		str = "public key transport";
		break;
	case se_key_type::symmetric_key_agreement:
		str = "symmetric key agreement";
		break;
	case se_key_type::private_static_key_agreement:
		str = "private static key agreement";
		break;
	case se_key_type::public_static_key_agreement:
		str = "public static key agreement";
		break;
	case se_key_type::private_ephemeral_key_agreement:
		str = "private ephemeral key agreement";
		break;
	case se_key_type::public_ephemeral_key_agreement:
		str = "public ephemeral key agreement";
		break;
	case se_key_type::symmetric_authorization:
		str = "symmetric authorization";
		break;
	case se_key_type::private_authorization:
		str = "private authorization";
		break;
	case se_key_type::public_authorization:
		str = "public authorization";
		break;
	default:
		str = "invalid key type";
	}
	return str;
}
uint32_t stoul_wrap(string& s){
	unsigned long x = stoul(s);
	if(x > UINT32_MAX){
		throw out_of_range("Value exceeds UINT32_MAX.");
	}
	return static_cast<uint32_t>(x);
}
string algomap(uint32_t algo){
	string algorithm;
	switch(algo){
		case L1Algorithms::Algorithms::AES_HMACSHA256:
			algorithm = "AES_HMACSHA256";
			break;
		case L1Algorithms::Algorithms::AES:
			algorithm = "AES";
			break;
		case L1Algorithms::Algorithms::SHA256:
			algorithm = "SHA256";
			break;
		case L1Algorithms::Algorithms::HMACSHA256:
			algorithm = "HMACSHA256";
			break;
		default:
			algorithm = "invalid algorithm";
	}
	return algorithm;
}
string cryptoperiod_to_days(uint32_t n){
	int day = n / (24 * 3600);
	n = n % (24 * 3600);
	int hour = n / 3600;
	n %= 3600;
	int minutes = n / 60 ;
	n %= 60;
	int seconds = n;
	string s = to_string(day) + " days, " + to_string(hour) + " hours, " + to_string(minutes) + " minutes, " + to_string(seconds) + " seconds.";
	return s;
}
string epoch_to_localtime(time_t t){
	string s;
	char c[100];
	memset(c, 0, 100);
	if(t == 0){
		s = "value not set (0)";
	} else {
		struct tm *tm_ = gmtime(&t);
		if(strftime(c, 100, "%c", tm_) == 0){
			s = "error in strftime()";
		} else {
			s = (const char*)c;
		}
	}
	s.erase(remove(s.begin(), s.end(), '\n'), s.end());
	return s;
}
int get_microsd_path(L0& l0, string& microsd){
#if defined(__linux__) || defined(__APPLE__)
		char *c = (char*)l0.GetDevicePath(); // this is always NULL terminated
		microsd = string(c);
		if(microsd.back() != '/'){
			microsd.push_back('/'); // add slash if required
		}
#elif _WIN32
		char path[L0Communication::Parameter::SE3_MAX_PATH];
		memset(path, '\0', L0Communication::Parameter::SE3_MAX_PATH);
		size_t retval;
		if(wcstombs_s(&retval, path, L0Communication::Parameter::SE3_MAX_PATH, l0.GetDevicePath(), L0Communication::Parameter::SE3_MAX_PATH-1) != 0){ // convert from wchar_t
			return -1;
		}
		microsd = string(path);
		if(microsd.back() != '\\'){
			microsd.push_back('\\'); // add backslash if required
		}
#endif
	return 0;
}

/* ITERATOR METHODS */
void sql_update_iterator(vector<string>& users, string& query, bool erase){
	if(!is_admin){ return; }
	try{
		for(string& curr_user : users){
			send_sql_update(curr_user, query, erase);
		}
	} catch(...){
		/* don't care, this method MUST be no throw to avoid problems in the API at higher levels. if an exception happens here, the caller has already
		 * assumed all the users passed as parameter will need a recovery. so don't bother handling exceptions because we already put ourselves in the
		 * worst case scenario, which basically means: remove the user from the recovery list only if the update was written correctly */
	}
}
void key_update_iterator(vector<string>& users, uint32_t kid, uint32_t key_len, bool erase){
	if(!is_admin){ return; }
	try{
		for(string& curr_user : users){
			send_key_update(curr_user, kid, key_len, erase);
		}
	} catch(...){
		/* don't care, this method MUST be no throw to avoid problems in the API at higher levels. if an exception happens here, the caller has already
		 * assumed all the users passed as parameter will need a recovery. so don't bother handling exceptions because we already put ourselves in the
		 * worst case scenario, which basically means: remove the user from the recovery list only if the update was written correctly */
	}
}
void delete_user_from_group_iterator(vector<string>& users, string& user_id, string& group_id, bool erase){
	if(!is_admin){ return; }
	try{
		for(string& curr_user : users){
			req_delete_user_from_group(curr_user, user_id, group_id, erase);
		}
	} catch(...){
		/* don't care, this method MUST be no throw to avoid problems in the API at higher levels. if an exception happens here, the caller has already
		 * assumed all the users passed as parameter will need a recovery. so don't bother handling exceptions because we already put ourselves in the
		 * worst case scenario, which basically means: remove the user from the recovery list only if the update was written correctly */
	}
}
void delete_user_iterator(vector<string>& users, string& user_id, bool erase){
	if(!is_admin){ return; }
	try{
		for(string& curr_user : users){
			req_delete_user(curr_user, user_id, erase);
		}
	} catch(...){
		/* don't care, this method MUST be no throw to avoid problems in the API at higher levels. if an exception happens here, the caller has already
		 * assumed all the users passed as parameter will need a recovery. so don't bother handling exceptions because we already put ourselves in the
		 * worst case scenario, which basically means: remove the user from the recovery list only if the update was written correctly */
	}
}
void delete_group_iterator(vector<string>& users, string& group_id, bool erase){
	if(!is_admin){ return; }
	try{
		for(string& curr_user : users){
			req_delete_group(curr_user, group_id, erase);
		}
	} catch(...){
		/* don't care, this method MUST be no throw to avoid problems in the API at higher levels. if an exception happens here, the caller has already
		 * assumed all the users passed as parameter will need a recovery. so don't bother handling exceptions because we already put ourselves in the
		 * worst case scenario, which basically means: remove the user from the recovery list only if the update was written correctly */
	}
}

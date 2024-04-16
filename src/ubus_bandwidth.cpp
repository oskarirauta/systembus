#include "common.hpp"
#include "logger.hpp"
#include "json.hpp"
#include "ubus.hpp"
#include "ubus_bandwidth.hpp"

static JSON get_bm(const bandwidth_t::interface_t& b) {

	JSON j_rx = JSON::Object();
	JSON j_rx_rate = JSON::Object();
	JSON j_tx = JSON::Object();
	JSON j_tx_rate = JSON::Object();

	unsigned long long rx_b = b.rx_bytes();
	unsigned long long rx_kb = rx_b / 1024;
	unsigned long long rx_mb = rx_kb / 1024;
	unsigned long long rx_gb = rx_mb / 1024;

	bandwidth_t::bps_t rx_rate = b.rx();
	bandwidth_t::bps_t rx_bps = b.rx_bps();

	unsigned long long tx_b = b.tx_bytes();
	unsigned long long tx_kb = tx_b / 1024;
	unsigned long long tx_mb = tx_kb / 1024;
	unsigned long long tx_gb = tx_mb / 1024;

	bandwidth_t::bps_t tx_rate = b.tx();
	bandwidth_t::bps_t tx_bps = b.tx_bps();

	bandwidth_t::bps_t rx_max = b.max_rx_bps();
	bandwidth_t::bps_t tx_max = b.max_tx_bps();

	bandwidth_t::percent_t percent = b.percent();

	if ( rx_bps.kb < 1 ) percent.rx = 0;
	if ( tx_bps.kb < 1 ) percent.tx = 0;

	std::string rx_value_type = " bps";
	if ( rx_bps.type() == bandwidth_t::bps_t::TYPE::K ) rx_value_type = " Kbps";
	else if ( rx_bps.type() == bandwidth_t::bps_t::TYPE::M ) rx_value_type = " Mbps";
	else if ( rx_bps.type() == bandwidth_t::bps_t::TYPE::G ) rx_value_type = " Gbps";

	std::string tx_value_type = " bps";
	if ( tx_bps.type() == bandwidth_t::bps_t::TYPE::K ) tx_value_type = " Kbps";
	else if ( tx_bps.type() == bandwidth_t::bps_t::TYPE::M ) tx_value_type = " Mbps";
	else if ( tx_bps.type() == bandwidth_t::bps_t::TYPE::G ) tx_value_type = " Gbps";

	std::string rx_max_type = " bps";
	if ( rx_max.type() == bandwidth_t::bps_t::TYPE::K ) rx_max_type = " Kbps";
	else if ( rx_max.type() == bandwidth_t::bps_t::TYPE::M ) rx_max_type = " Mbps";
	else if ( rx_max.type() == bandwidth_t::bps_t::TYPE::G ) rx_max_type = " Gbps";

	std::string tx_max_type = " bps";
	if ( tx_max.type() == bandwidth_t::bps_t::TYPE::K ) tx_max_type = " Kbps";
	else if ( tx_max.type() == bandwidth_t::bps_t::TYPE::M ) tx_max_type = " Mbps";
	else if ( tx_max.type() == bandwidth_t::bps_t::TYPE::G ) tx_max_type = " Gbps";

	j_rx["bytes"] = rx_b;
	j_rx["kilobytes"] = rx_kb;
	j_rx["megabytes"] = rx_mb;
	j_rx["gigabytes"] = rx_gb;
	j_rx["packets"] = b.rx_packets();
	j_rx["errors"] = b.rx_errors();

	j_rx_rate["Bytes"] = rx_rate.bytes;
	j_rx_rate["KBytes"] = rx_rate.kb;
	j_rx_rate["MBytes"] = rx_rate.mb;
	j_rx_rate["GBytes"] = rx_rate.gb;
	j_rx_rate["b"] = rx_bps.bytes;
	j_rx_rate["k"] = rx_bps.kb;
	j_rx_rate["m"] = rx_bps.mb;
	j_rx_rate["g"] = rx_bps.gb;
	j_rx_rate["value"] = common::to_string(rx_bps.value()) + rx_value_type;
	j_rx_rate["max"] = common::to_string(rx_max.value()) + rx_max_type;
	j_rx_rate["percent"] = percent.rx;

	j_rx["rate"] = j_rx_rate;

	j_tx["bytes"] = tx_b;
	j_tx["kilobytes"] = tx_kb;
	j_tx["megabytes"] = tx_mb;
	j_tx["gigabytes"] = tx_gb;
	j_tx["packets"] = b.tx_packets();
	j_tx["errors"] = b.tx_errors();

	j_tx_rate["Bytes"] = tx_rate.bytes;
	j_tx_rate["KBytes"] = tx_rate.kb;
	j_tx_rate["MBytes"] = tx_rate.mb;
	j_tx_rate["GBytes"] = tx_rate.gb;
	j_tx_rate["b"] = tx_bps.bytes;
	j_tx_rate["k"] = tx_bps.kb;
	j_tx_rate["m"] = tx_bps.mb;
	j_tx_rate["g"] = tx_bps.gb;
	j_tx_rate["value"] = common::to_string(tx_bps.value()) + tx_value_type;
	j_tx_rate["max"] = common::to_string(tx_max.value()) + tx_max_type;
	j_tx_rate["percent"] = percent.tx;

	j_tx["rate"] = j_tx_rate;

	JSON ifd = JSON::Object();
	ifd["rx"] = j_rx;
	ifd["tx"] = j_tx;

	return ifd;
}

int ubus_bandwidth_get(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "bandwidth::get called" << std::endl;

	if ( req.empty() || ( !req.contains("interface") && !req.contains("ifd" ))) {

		logger::verbose << "incorrect args for bandwidth::get" << std::endl;
		res["error"] = "network usage method get requires argument interface";
		return 0;
	}

	if (( req.contains("interface") && !req["interface"].convertible_to(JSON::STRING)) ||
		( req.contains("ifd") && !req["ifd"].convertible_to(JSON::STRING))) {

		logger::verbose << "missing args for bandwidth::get" << std::endl;
		res["error"] = "network usage method get did not receive interface argument in correct format";
		return 0;
	}

	std::string ifd = req.contains("interface") ? req["interface"].to_string() : ( req.contains("ifd") ? req["ifd"].to_string() : "" );

	if ( ifd.empty()) {

		logger::verbose << "syntax error with bandwidth::get args" << std::endl;
		res["error"] = "network usage method get received empty string as interface argument";
		return 0;
	}

	for ( const auto& b : bm -> interfaces()) {

		if ( b.name() != ifd )
			continue;

		res[b.name()] = get_bm(b);
		return 0;
	}

	logger::verbose << "failure with bandwidth::get, interface " << ifd << " was not found" << std::endl;
	res["error"] = "interface " + ifd + " not found";

	return 0;
}

int ubus_bandwidth_list(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "bandwidth::list called" << std::endl;

	for ( const auto& b : bm -> interfaces()) {

		res[b.name()] = get_bm(b);
	}

	return 0;
};

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_CLIENT_H_
#define BAT_CLIENT_H_

#include <string>
#include <vector>
#include <mutex>

#include "base/callback.h"

#include "bat_balance.h"
#include "bat_client_webrequest.h"
#include "bat_helper.h"

namespace bat_client {

class BatClient {
public:

  BatClient(const bool& useProxy = true);
  ~BatClient();

  void loadStateOrRegisterPersona();
  void requestCredentialsCallback(bool result, const std::string& response, const FETCH_CALLBACK_EXTRA_DATA_ST& extraData);
  void registerPersonaCallback(bool result, const std::string& response, const FETCH_CALLBACK_EXTRA_DATA_ST& extraData);
  void publisherTimestampCallback(bool result, const std::string& response, const FETCH_CALLBACK_EXTRA_DATA_ST& extraData);
  uint64_t getPublisherTimestamp();
  void publisherInfo(const std::string& publisher, BatHelper::FetchCallback callback,
    const FETCH_CALLBACK_EXTRA_DATA_ST& extraData);
  void setContributionAmount(const double& amount);

  std::string getBATAddress();
  std::string getBTCAddress();
  std::string getETHAddress();
  std::string getLTCAddress();

  void getWalletProperties(BatHelper::FetchCallback callback,
    const FETCH_CALLBACK_EXTRA_DATA_ST& extraData);

private:
  void loadStateOrRegisterPersonaCallback(bool result, const CLIENT_STATE_ST& state);
  void registerPersona();
  void publisherTimestamp(const bool& saveState = true);

  std::string buildURL(const std::string& path, const std::string& prefix);

  bool useProxy_;
  BatClientWebRequest batClientWebRequest_;
  CLIENT_STATE_ST state_;
  std::string preFlight_;
  uint64_t publisherTimestamp_;
  std::mutex state_mutex_;
  bat_balance::BatBalance balance_;
};
}

#endif  // BAT_CLIENT_H_

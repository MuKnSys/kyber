#include "ShuffleRoundBlame.hpp"

#include "../Crypto/CryptoFactory.hpp"
#include "../Connections/EmptyNetwork.hpp"

namespace Dissent {
namespace Anonymity {
  EmptyGetDataCallback ShuffleRoundBlame::_empty_get_data;

  ShuffleRoundBlame::ShuffleRoundBlame(QSharedPointer<GroupGenerator> group_gen,
      const Id &local_id, const Id &round_id, AsymmetricKey *outer_key) :
    ShuffleRound(group_gen, local_id, round_id, EmptyNetwork::GetInstance(),
        QSharedPointer<AsymmetricKey>(), _empty_get_data)
  {
    if(outer_key) {
      Library *lib = CryptoFactory::GetInstance().GetLibrary();
      _outer_key.reset(lib->LoadPrivateKeyFromByteArray(outer_key->GetByteArray()));
    }
    _log.ToggleEnabled();
  }

  bool ShuffleRoundBlame::Start()
  {
    QScopedPointer<AsymmetricKey> tmp(_outer_key.take());
    bool nstarted = ShuffleRound::Start();
    _outer_key.reset(tmp.take());
    return !nstarted;
  }

  int ShuffleRoundBlame::GetGo(int idx)
  {
    if(_go_received[idx]) {
      return _go[idx] ? 1 : - 1;
    }
    return 0;
  }

  void ShuffleRoundBlame::BroadcastPublicKeys()
  {
    _state = KeySharing;
  }

  void ShuffleRoundBlame::SubmitData()
  {
    if(_shuffler) {
      _state = WaitingForShuffle;
    } else {
      _state = ShuffleRound::WaitingForEncryptedInnerData;
    }
  }

  void ShuffleRoundBlame::Shuffle()
  {
    _state = ShuffleRound::Shuffling;

    OnionEncryptor *oe = CryptoFactory::GetInstance().GetOnionEncryptor();
    oe->Decrypt(_outer_key.data(), _shuffle_ciphertext, _shuffle_cleartext,
        &_bad_members);

    _state = ShuffleRound::WaitingForEncryptedInnerData;
  }

  void ShuffleRoundBlame::Verify()
  {
  }

  void ShuffleRoundBlame::StartBlame()
  {
  }

  void ShuffleRoundBlame::BroadcastPrivateKey()
  {
    _state = PrivateKeySharing;
  }

  void ShuffleRoundBlame::Decrypt()
  {
  }

  void ShuffleRoundBlame::BlameRound()
  {
  }
}
}

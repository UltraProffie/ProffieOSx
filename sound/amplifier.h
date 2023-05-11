#ifndef SOUND_AMPLIFIER_H
#define SOUND_AMPLIFIER_H

#ifdef ENABLE_AUDIO

#include "dac.h"

#ifdef AUDIO_CONTROL_SGTL5000
#include <control_sgtl5000.h>
AudioControlSGTL5000     sgtl5000_1;
bool sgtl5000_enabled = false;
#endif


// Turns off amplifier when no audio is played.
// Maybe name this IdleHelper or something instead??
class Amplifier : Looper, StateMachine, CommandParser {
public:
  Amplifier() : Looper(), CommandParser() {}

  const char* name() override { return "Amplifier"; }

  bool Active() {
//    if (saber_synth.on_) return true;
//    if (audio_splicer.isPlaying()) return true;
    if (beeper.isPlaying()) return true;
    if (talkie.isPlaying()) return true;
    for (size_t i = 0; i < NELEM(wav_players); i++)
      if (wav_players[i].isPlaying())
        return true;
    uint32_t t = millis() - last_enabled_;
    if (t < 30) return true;
    return false;
  }

  void Enable() {
    // dac.begin();
    last_enabled_ = millis();
#ifdef AUDIO_CONTROL_SGTL5000
    if (!sgtl5000_enabled) {
      sgtl5000_1.enable();
      sgtl5000_1.volume(0.7);
      sgtl5000_enabled = true;
    }
#else    
    if (!on_) {
      on_ = true;
      EnableBooster();
      // pinMode(amplifierPin, OUTPUT);
      // digitalWrite(amplifierPin, HIGH);
      // delay(10); // Give it a little time to wake up.
      // #ifdef ULTRA_PROFFIE
      //    stm32l4_gpio_pin_configure(GPIO_PIN_PB2, GPIO_MODE_OUTPUT | GPIO_OTYPE_PUSHPULL | GPIO_OSPEED_LOW | GPIO_PUPD_PULLDOWN);
      // //   digitalWrite(16, LOW);
      // //   delay(500); // Give it a little time to wake up.
      // #endif
      // // delay(10); // Give it a little time to wake up.
    }
#endif    
  }

protected:
  void Setup() override {
    // Audio setup
#ifndef AUDIO_CONTROL_SGTL5000
    // pinMode(amplifierPin, OUTPUT);
#endif    
    SetupStandardAudio();
    last_enabled_ = millis();
  }

  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (Active())
      {

       YIELD();
      }

      // SLEEP(20);
      SLEEP(50);
      
      if (Active()) continue;
      #if (defined(OSx) && defined(DIAGNOSE_AUDIO)) || !defined(OSx)  
      STDOUT.println("Amplifier off.");
      #endif
      on_ = false;
      // digitalWrite(amplifierPin, LOW); // turn the amplifier off
#ifdef AUDIO_CONTROL_SGTL5000
      // Disable does nothing, so this is pointless.
      // sgtl5000_1.volume(0.0);
      // sgtl5000_1.disable();
      // sgtl5000_enabled = false;
#else
  // #ifndef ULTRA_PROFFIE
  //     pinMode(amplifierPin, INPUT_ANALOG); // Let the pull-down do the work
  // #else 
  //     digitalWrite(amplifierPin, LOW); // turn the amplifier off
  //     // TODO DELEte
  //     //stm32l4_gpio_pin_configure(GPIO_PIN_PB2, GPIO_MODE_ANALOG | GPIO_OTYPE_PUSHPULL | GPIO_OSPEED_LOW | GPIO_PUPD_PULLDOWN);  // GPIO_MODE_OUTPUT
  // #endif
#endif      
      SLEEP(20);
      if (on_) continue;
      dac.end();
      while (!Active()) YIELD();
    }
    STATE_MACHINE_END();
  }


  bool Parse(const char *cmd, const char* arg) override {
    if (!strcmp(cmd, "amp")) {
      if (!strcmp(arg, "on")) {
	Enable();
        return true;
      }
      if (!strcmp(arg, "off")) {
#ifdef AUDIO_CONTROL_SGTL5000
	sgtl5000_1.volume(0.0);
	sgtl5000_1.disable();
#else
  // #ifndef ULTRA_PROFFIE
  //     pinMode(amplifierPin, INPUT_ANALOG); // Let the pull-down do the work
  // #else 
  //     digitalWrite(amplifierPin, LOW); // turn the amplifier off
  // #endif
#endif      
        return true;
      }
    }
// #ifndef DISABLE_DIAGNOSTIC_COMMANDS
#ifdef ENABLE_DEVELOPER_COMMANDS
    if (!strcmp(cmd, "whatison")) {
      bool on = false;
      SaberBase::DoIsOn(&on);
      STDOUT.print("Saber bases: ");
      STDOUT.println(on ? "On" : "Off");
//      STDOUT.print("Audio splicer: ");
//      STDOUT.println(audio_splicer.isPlaying() ? "On" : "Off");
      STDOUT.print("Beeper: ");
      STDOUT.println(beeper.isPlaying() ? "On" : "Off");
      STDOUT.print("Talker: ");
      STDOUT.println(talkie.isPlaying() ? "On" : "Off");
      for (size_t i = 0; i < NELEM(wav_players); i++) {
	STDOUT << "Wav player " << i << ": "
	       << (wav_players[i].isPlaying() ? "On" : "Off")
	       << " (eof =  " << wav_players[i].eof()
	       << " volume = " << wav_players[i].volume()
	       << " refs = " << wav_players[i].refs()
	       << " fade speed = " << wav_players[i].fade_speed()
	       << " filename=" << wav_players[i].filename() << ")\n";
      }
      return true;
    }
#endif    
    return false;
  }

  void Help() {
    #if defined(COMMANDS_HELP) || !defined(OSx)
    STDOUT.println(" amp on/off - turn amplifier on or off");
    #endif
  }

private:
  bool on_ = false;
  uint32_t last_enabled_;
};

Amplifier amplifier;

// void EnableAmplifier() {
  // amplifier.Enable();
//   dynamic_mixer.RequestPower();
// }
bool AmplifierIsActive() {
  return amplifier.Active();
}
#ifdef ULTRA_PROFFIE
void SilentEnableAmplifier(bool on) {
  // if(on)
  //       digitalWrite(amplifierPin, HIGH); // turn the amplifier off
  // else 
  //       digitalWrite(amplifierPin, LOW); // turn the amplifier off
}
#endif

#else // ENABLE_AUDIO
void EnableAmplifier() { }
bool AmplifierIsActive() { return false; }
#ifdef ULTRA_PROFFIE
void SilentEnableAmplifier(bool on) { }
#endif

#endif   // ENABLE_AUDIO
#endif

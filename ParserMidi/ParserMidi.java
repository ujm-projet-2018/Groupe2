package projtut;

import java.io.File;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;

import javax.sound.midi.MidiEvent;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.Sequence;
import javax.sound.midi.ShortMessage;
import javax.sound.midi.Track;

public class ParserMidi {
	public static final int NOTE_ON = 0x90;
	public static final int NOTE_OFF = 0x80;
	public static final String[] NOTE_NAMES = { "c 0", "c 1", "d 0", "d 1", "e 0", "f 0", "f 1", "g 0", "g 1", "a 0",
			"a 1", "b 0" };
	public static long notesTab[] = new long[129];

	public static void main(String[] args) throws Exception {
		Sequence sequence = MidiSystem.getSequence(new File(args[0]));
		for (int i = 0; i < 129; i++)
			notesTab[i] = 0;

		Charset utf8 = StandardCharsets.UTF_8;
		Path path = Paths.get("notes.txt");

		String str = "";
		int trackNumber = 0, duree;
		long deltaTick, ecart;
		long tick = 0, lastTick = 0;
		int accord = 0;
		deltaTick=ecart=0;

		for (Track track : sequence.getTracks()) {
			trackNumber++;
			System.out.println("Track " + trackNumber + ": size = " + track.size());
			System.out.println();

			int nbNotes = 0;

			for (int j = 0; j < track.size(); j++) {
				MidiEvent event = track.get(j);
				MidiMessage message = event.getMessage();
				if (message instanceof ShortMessage) {
					ShortMessage sm = (ShortMessage) message;
					if (sm.getCommand() == NOTE_ON && sm.getData2() > 0)
						nbNotes++;
				}
			}
			System.out.println(nbNotes);
			str = Integer.toString(nbNotes) + "\n" + "4 4" + "\n";
			Files.write(path, str.getBytes(utf8), StandardOpenOption.CREATE);

			double resolution = sequence.getResolution();
			double ticksPerSecond = resolution * (120 / 60.0);
			double tickSize = 1.0 / ticksPerSecond;
			System.out.println("ticksize :" + tickSize);
			System.out.println("resolution :" + sequence.getResolution());

			for (int i = 0; i < track.size(); i++) {

				MidiEvent event = track.get(i);

				lastTick = tick;
				tick = event.getTick();

				System.out.print("@" + event.getTick() + " ");
				MidiMessage message = event.getMessage();

				if (message instanceof ShortMessage) {
					ShortMessage sm = (ShortMessage) message;
					System.out.print("Channel: " + sm.getChannel() + " ");

					if (sm.getCommand() == NOTE_ON) {
						int key = sm.getData1();
						int octave = (key / 12) - 1;
						int note = key % 12;
						String noteName = NOTE_NAMES[note];
						int velocity = sm.getData2();
						deltaTick = 0;

						// calcul de la duree
						if (velocity != 0) {
							notesTab[key] = tick;

						} else {
							deltaTick = tick - notesTab[key];
							notesTab[key] = 0;
							
							ecart = tick - lastTick;
							if (ecart != 0) {

								accord = (accord+1)%2;
							}
						}

						// note octave dièse temps silence durée

						if (deltaTick != 0) {
							duree =  (int) (2 / (deltaTick * tickSize));
							str += noteName + " " + octave + " " + duree + " 0 " + accord+" ";
							str += "\n";
						}

						System.out.println("Note on, " + noteName + octave + " key=" + key + " velocity: " + velocity);
					} else {
						System.out.println("Command:" + sm.getCommand() + " " + sm.getData1() + " " + sm.getData2());
					}
				} else {
					StringBuilder tempo = new StringBuilder();
					byte[] tempobytes = message.getMessage();

					/*
					 * for(int k = 3; k < message.getLength();k++) {
					 * tempo.append(String.format("%02X", tempobytes[k])); }
					 */

					for (byte b : message.getMessage()) {
						tempo.append(String.format("%02X ", b));
					}
					System.out.println("Other message: " + message.getClass() + " " + message.getStatus() + " "
							+ tempo.toString() + " ");
				}
				Files.write(path, str.getBytes(utf8), StandardOpenOption.CREATE, StandardOpenOption.TRUNCATE_EXISTING);
			}
			System.out.println();
		}

	}
}
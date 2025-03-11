/**
 * @function git_hash_object
 *
 * Hash the contents of a source file the way git would do it so that
 * `git cat-file -p HASH` will generally find the appropriate source
 * file contents. There should be other benefits like looking through
 * logs to find the exact file using these hashes.
 *
 * This may seem a bit like "signing", and while it may be a step in
 * that direction, it shouldn't been seen as so. There may be
 * interesting forensic steps could be done, based only on this data
 * and "official" repositories, that could help trace back issues.
 *
 * For files that come from other repositories (our builds depend on
 * c-armyknife-lib which IS in a different repo), I'm not sure what
 * will happen attempting to use these hashes. git hash-object seems
 * to be willing to hash anything even if not under git control and it
 * *probably* defaults (or always) uses the hash algorithm of the repo
 * it sees as "cwd" rather than the file it comes from though I could
 * be very wrong.
 */
buffer_t* git_hash_object(char* filename) {
  value_array_t* argv = make_value_array(2);
  value_array_add(argv, str_to_value("git"));
  value_array_add(argv, str_to_value("hash-object"));
  value_array_add(argv, str_to_value(filename));

  sub_process_t* sub_process = make_sub_process(argv);
  sub_process_launch(sub_process);

  buffer_t* buffer = make_buffer(1);
  do {
    sub_process_read(sub_process, buffer, nullptr);
    usleep(5);
  } while (is_sub_process_running(sub_process));
  sub_process_read(sub_process, buffer, nullptr);
  sub_process_wait(sub_process);

  return buffer;
}

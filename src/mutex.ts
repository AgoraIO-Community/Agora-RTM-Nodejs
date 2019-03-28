export default class Mutex {
  private lockers = new Map<string, boolean>();

  public lock(key: string) {
    this.lockers.set(key, true);
  }

  public unlock(key: string) {
    this.lockers.set(key, false)
  }

  public wait(key: string) {
    return new Promise((resolve, reject) => {
      setTimeout(() => {
        if (!this.lockers.get(key)) {
          resolve()
        } else {
          reject(new Error(`${key} has been locked`))
        }
      })
    })
  }
}
/**
 * Class used for loading and interacting with the rendered objects.
 * @author Vlad Pirlog <https://github.com/vladpirlog>
 * @see https://github.com/AntonVonDelta/MDSProiect
 */
class SceneRenderer {
    /**
     * RegExp object used for validating `.obj` input strings.
     * @private
     * @static
     */
    private static _OBJ_LINE_REGEX = /^(v( -?(0|[1-9]\d*)(\.\d+)?){3,4}|vn( -?(0|[1-9]\d*)(\.\d+)?){3}|f( [1-9]\d*((\/([1-9]\d*)?)?\/[1-9]\d*)?){3,})$/

    /**
     * Map for translating moving directions into API opcodes.
     * @private
     * @static
     */
    private static _MOVE_DIRECTION_OPCODES = {
        positiveX: 2 as const,
        negativeX: 3 as const,
        positiveY: 4 as const,
        negativeY: 5 as const,
        positiveZ: 0 as const,
        negativeZ: 1 as const,
    }

    /**
     * Map for translating rotation directions into API opcodes.
     * @private
     * @static
     */
    private static _ROTATE_DIRECTION_OPCODES = {
        aroundX: 1 as const,
        aroundY: 0 as const
    }

    /**
     * Map used for storing the names of the custom HTTP headers used.
     * @private
     * @static
     */
    private static _HEADERS = {
        WIDTH: 'X-Image-Width',
        HEIGHT: 'X-Image-Height',
        AUTHORIZATION: 'X-Authorize'
    }

    /**
     * Flag used for signaling current user's authentication status.
     * @private
     */
    private _loggedIn = false

    /**
     * The URL origin for the API calls.
     * @private
     */
    private _origin: string

    /**
     * Map for translating actions into API endpoints.
     * @private
     */
    private _endpoints: { login: string, move: string, rotate: string, load: string }

    /**
     * The API key received on login. Used for subsequent requests made to the server.
     * @private
     */
    private _apiKey: string | null = null

    /**
     * @param origin the URL origin for the API calls
     */
    public constructor(origin = '') {
        this._origin = origin
        this._endpoints = {
            login: `${this._origin}/api/login`,
            move: `${this._origin}/api/move`,
            rotate: `${this._origin}/api/rotate`,
            load: `${this._origin}/api/load`
        }
    }

    /**
     * Function that authenticates the user in order to start the video feed.
     * @returns a promise that resolves to an object with `width`, `height` (the size of the image) and
     * `stream` (null or a ReadableStream for an UInt8Array) properties
     * @throws `AlreadyLoggedInError`, `CannotGetKeyError`, `UnknownStatusCodeError`, `InternalServerError`
     * @async
     */
    public async login () {
        if (this._loggedIn) throw new AlreadyLoggedInError()

        const res = await fetch(this._endpoints.login)
        if (res.status === 500) throw new InternalServerError(await res.text())
        if (res.status === 409) throw new CannotGetKeyError()
        if (res.status !== 200) throw new UnknownStatusCodeError()

        this._loggedIn = true
        if (!res.headers.has(SceneRenderer._HEADERS.AUTHORIZATION))
            throw new CannotGetKeyError()
        this._apiKey = res.headers.get(SceneRenderer._HEADERS.AUTHORIZATION)
        const imageWidthHeader = res.headers.get(SceneRenderer._HEADERS.WIDTH)
        const imageHeightHeader = res.headers.get(SceneRenderer._HEADERS.HEIGHT)

        const imageWidth = imageWidthHeader ? parseInt(imageWidthHeader) : 720
        const imageHeight = imageHeightHeader ? parseInt(imageHeightHeader) : 480
        const imageReadableStream = res.body

        return { width: imageWidth, height: imageHeight, stream: imageReadableStream }
    }

    /**
     * Function that sends a `.obj` formatted string to the server in order to render it.
     * @param data a string representing the user input in a `.obj` format
     * @returns a promise that resolves to `true` if the request succeeded, else throws an error
     * @throws `UnauthorizedError`, `MalformedDataError`, `UnknownStatusCodeError`, `InternalServerError`
     * @see https://www.cs.cmu.edu/~mbz/personal/graphics/obj.html
     * @async
     */
    public async loadFromText (data: string) {
        if (!this._loggedIn || !this._apiKey) throw new UnauthorizedError()

        const validLines = data
            .split('\n')
            .map(line => line.trim())
            .filter(line => line.length > 0 && !line.startsWith('#'))
        if (!validLines.every(line => SceneRenderer._OBJ_LINE_REGEX.test(line))) {
            throw new MalformedDataError()
        }

        const res = await fetch(this._endpoints.load, {
            body: data,
            method: 'POST',
            headers: {
                'Content-Type': 'text/plain',
                [SceneRenderer._HEADERS.AUTHORIZATION]: this._apiKey
            }
        })
        if (res.status === 500) throw new InternalServerError(await res.text())
        if (res.status === 404) {
            this._loggedIn = false
            this._apiKey = null
            throw new UnauthorizedError()
        }
        if (res.status === 422) {
            throw new MalformedDataError(await res.text())
        }
        if (res.status !== 200) throw new UnknownStatusCodeError()
        return true
    }

    /**
     * Function that sends a `move` command to the server in order to move the rendered object.
     * @param direction positive or negative movement along one of the 3 axes
     * @param amount number of units to move in the given direction
     * @returns a promise that resolves to `true` if the request succeeded, else throws an error
     * @throws `UnauthorizedError`, `UnknownStatusCodeError`, `InternalServerError`
     * @async
     */
    public async move (direction: keyof typeof SceneRenderer._MOVE_DIRECTION_OPCODES, amount: number) {
        if (!this._loggedIn || !this._apiKey) throw new UnauthorizedError()

        const queryParams = new URLSearchParams()
        queryParams.append('direction', SceneRenderer._MOVE_DIRECTION_OPCODES[direction].toString())
        queryParams.append('amount', amount.toString())

        const fullPath = this._endpoints.move + '?' + queryParams.toString()

        const res = await fetch(fullPath, { headers: { [SceneRenderer._HEADERS.AUTHORIZATION]: this._apiKey || '' } })
        if (res.status === 500) throw new InternalServerError(await res.text())
        if (res.status === 404) {
            this._loggedIn = false
            this._apiKey = null
            throw new UnauthorizedError()
        }
        if (res.status !== 200) throw new UnknownStatusCodeError()
        return true
    }

    /**
     * Function that sends a `rotate` command to the server in order to rotate the rendered object.
     * @param direction the axis of rotation
     * @param amount number of degrees to rotate the object by; positive means counter-clockwise rotation
     * and negative means clockwise rotation
     * @returns a promise that resolves to `true` if the request succeeded, else throws an error
     * @throws `UnauthorizedError`, `UnknownStatusCodeError`, `InternalServerError`
     * @async
     */
    public async rotate (direction: keyof typeof SceneRenderer._ROTATE_DIRECTION_OPCODES, amount: number) {
        if (!this._loggedIn || !this._apiKey) throw new UnauthorizedError()

        const queryParams = new URLSearchParams()
        queryParams.append('direction', SceneRenderer._ROTATE_DIRECTION_OPCODES[direction].toString())
        queryParams.append('amount', amount.toString())

        const fullPath = this._endpoints.rotate + '?' + queryParams.toString()

        const res = await fetch(fullPath, { headers: { [SceneRenderer._HEADERS.AUTHORIZATION]: this._apiKey || '' } })
        if (res.status === 500) throw new InternalServerError(await res.text())
        if (res.status === 404) {
            this._loggedIn = false
            this._apiKey = null
            throw new UnauthorizedError()
        }
        if (res.status !== 200) throw new UnknownStatusCodeError()
        return true
    }
}

class UnauthorizedError extends Error {
    constructor (message = 'Unauthorized') {
        super(message)
    }
}

class AlreadyLoggedInError extends Error {
    constructor (message = 'Already Logged In') {
        super(message)
    }
}

class UnknownStatusCodeError extends Error {
    constructor (message = 'Unknown Status Code') {
        super(message)
    }
}

class CannotGetKeyError extends Error {
    constructor (message = 'Cannot Get API Key') {
        super(message)
    }
}

class MalformedDataError extends Error {
    constructor (message = 'Malformed Data') {
        super(message)
    }
}

class InternalServerError extends Error {
    constructor (message = 'Internal Server Error') {
        super(message)
    }
}
